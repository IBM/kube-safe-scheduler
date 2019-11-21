/*
 * cloudsystem.c
 *
 *  Created on: Feb 10, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "cloudsystem.h"
#include "pe.h"

#include "../utils/hashmap.h"
#include "../utils/mathhelper.h"
#include "../utils/matrix.h"


/**
 * A model of the cloud system
 */
struct s_cloudsystem {
	/** physical entities in the system */
	vector* pes;

	/** a hashmap of PEs in the system peID->pe */
	hashmap* pemap;

	/** LE demand statistics */
	demandstats* ds;

	matrix* orderedutil;
	int orderedcount;

	/**
	 * toggle for dynamic addition and/or deletion of PEs
	 * (if the system is modified, then some indices and pointers, needed by placement
	 * functions, may be inconsistent until cloudsystem_resetpes() is called)
	 **/
	bool ismodified;
};


cloudsystem * cloudsystem_create(vector* _pes) {
	cloudsystem * cs = (void *) malloc(sizeof(cloudsystem));
	assert(cs);

	cs->pes = _pes;
	cs->pemap = hashmap_create(0);
	assert(cs->pemap);

	cs->ds = NULL;

	cs->orderedutil = NULL;
	cs->orderedcount = 0;

	cs->ismodified = true;
	cloudsystem_resetpes(cs);

	return cs;
}

void cloudsystem_destroy(cloudsystem* _self) {
	if (_self) {
		if (_self->pes) {
			for (int i = 0; i < vector_getsize(_self->pes); ++i) {
				pe * p = vector_get(_self->pes, i);
				pe_destroy(p);
			}

			vector_destroy(_self->pes);
			_self->pes = NULL;
		}

		hashmap_destroy(_self->pemap);

		if (_self->ds) {
			demandstats_destroy(_self->ds);
			_self->ds = NULL;
		}

		if (_self->orderedutil) {
			matrix_destroy(_self->orderedutil);
			_self->orderedutil = NULL;
		}

		free(_self);
	}
}

int cloudsystem_getnumPEs(cloudsystem * _self) {
	if (_self->pes) {
		return vector_getsize(_self->pes);
	}
	return 0;
}

vector * cloudsystem_getPEs(cloudsystem* _self) {
	return _self->pes;
}

void * cloudsystem_getPE(cloudsystem * _self, char* _peid) {
	return hashmap_get(_self->pemap, _peid);
}

void * cloudsystem_getPE_byindex(cloudsystem* _self, int _index) {
	return (void*) vector_get(_self->pes, _index);
}

char* cloudsystem_getPEname_byindex(cloudsystem* _self, int _index) {
	pe* pei = cloudsystem_getPE_byindex(_self, _index);
	if (pei) {
		return pe_getname(pei);
	}
	return NULL;
}

void cloudsystem_addPE(cloudsystem* _self, void* _pe) {
	pe* peadded = _pe;
	if (!peadded || !pe_getname(peadded) || cloudsystem_getPE(_self, pe_getname(peadded))) {
		return;
	}
	char* peid = pe_getname(peadded);
	hashmap_insert(_self->pemap, peadded, peid);
	if (!_self->pes) {
		_self->pes = vector_create();
	}
	vector_append(_self->pes, peadded);

	_self->ismodified = true;
}

void * cloudsystem_remove_PE(cloudsystem* _self, char* _peid) {
	pe* pedeleted = hashmap_get(_self->pemap, _peid);
	if (pedeleted) {
		// clear allocations on PE
		pe_clear(pedeleted, true);

		// remove the PE
		hashmap_remove(_self->pemap, _peid);
		vector_delete(_self->pes, pedeleted);

		_self->ismodified = true;
	}
	return pedeleted;
}

void cloudsystem_updatePE(cloudsystem* _self, char* _peid, int* _capacity, int* _usage) {
	pe* peupdated = cloudsystem_getPE(_self, _peid);
	if (peupdated) {
		pe_update(peupdated, _capacity, _usage);
	}
}

void cloudsystem_resetpes(cloudsystem* _self) {
	if (!_self->ismodified) {
		return;
	}
	_self->ismodified = false;

	// reset the map
	if (_self->pemap) {
		hashmap_destroy(_self->pemap);
	}
	_self->pemap = hashmap_create(0);

	// add all PEs
	if (_self->pes) {
		for (int i = 0; i < vector_getsize(_self->pes); i++) {
			pe * pei = vector_get(_self->pes, i);
			char* peid = pe_getname(pei);
			hashmap_insert(_self->pemap, pei, peid);
			pe_setindexinsystem(pei, i); //default
		}
	} else {
		_self->pes = vector_create();
		return;
	}
}

int cloudsystem_getnumresources(cloudsystem * _self) {
	if (cloudsystem_getnumPEs(_self) == 0) {
		return 0;
	}

	int numres = 0;
	pe * host0 = vector_get(_self->pes, 0);
	if (host0) {
		resource * r0 = pe_getresource(host0);
		if (r0) {
			numres = resource_getnum(r0);
		}
	}
	return numres;
}

float* cloudsystem_getutilization(cloudsystem * _self) {
	int numres = cloudsystem_getnumresources(_self);
	if (numres == 0) {
		return NULL;
	}

	float* avgutil = (float*) calloc(numres, sizeof(float));
	assert(avgutil);

	int numpes = cloudsystem_getnumPEs(_self);
	for (int i = 0; i < numpes; i++) {
		pe * host = vector_get(_self->pes, i);
		resource * r = pe_getresource(host);
		if (r) {
			float * u = resource_getutilization(r);
			mathhelper_addarray(avgutil, u, numres);
			free(u);
		}
	}
	mathhelper_scalearray(avgutil, numres, 1 / ((float) numpes));
	return avgutil;
}

float* cloudsystem_getutilizationstdev(cloudsystem * _self) {
	int numres = cloudsystem_getnumresources(_self);
	if (numres == 0) {
		return NULL;
	}

	int numpes = vector_getsize(_self->pes);
	float** u = (float**) malloc(numres * sizeof(float*));
	assert(u);
	for (int k = 0; k < numres; ++k) {
		u[k] = (float*) calloc(numpes, sizeof(float));
		assert(u[k]);
	}

	for (int i = 0; i < numpes; ++i) {
		pe * host = vector_get(_self->pes, i);
		resource * r = pe_getresource(host);
		if (r) {
			float* util = resource_getutilization(r);
			for (int k = 0; k < numres; ++k) {
				u[k][i] = util[k];
			}
			free(util);
		}
	}

	float* stdev = (float*) malloc(numres * sizeof(float));
	assert(stdev);
	for (int k = 0; k < numres; ++k) {
		stdev[k] = mathhelper_stddev(u[k], numpes);
		free(u[k]);
	}
	free(u);

	return stdev;
}

float* cloudsystem_getaveragecapacity(cloudsystem * _self) {
	int numres = cloudsystem_getnumresources(_self);
	if (numres == 0) {
		return NULL;
	}

	float* avgcap = (float*) calloc(numres, sizeof(float));
	assert(avgcap);
	int* count = (int*) calloc(numres, sizeof(int));
	assert(count);
	int numpes = cloudsystem_getnumPEs(_self);
	for (int i = 0; i < numpes; i++) {
		pe * host = vector_get(_self->pes, i);
		resource * r = pe_getresource(host);
		if (r) {
			int * capacity = resource_getcapacity(r);
			for (int j = 0; j < numres; ++j) {
				if (capacity[j] > 0) {
					avgcap[j] += capacity[j];
					count[j]++;
				}
			}
		}
	}
	for (int j = 0; j < numres; ++j) {
		if (count[j] > 0) {
			avgcap[j] /= count[j];
		}
	}
	free(count);
	return avgcap;
}

void cloudsystem_printutilizationmatrix(cloudsystem * _self) {
	int n = cloudsystem_getnumPEs(_self);
	int m = cloudsystem_getnumresources(_self);
	if (n == 0 || m == 0) {
		return;
	}

	/*
	 * initialization
	 */
	if (!_self->orderedutil) {
		_self->orderedutil = matrix_create(m, n);
		_self->orderedcount = 0;
	}

	/*
	 * collect system data
	 */
	matrix* sysM = matrix_create(m, n);
	float** u = matrix_getdata(sysM);

	vector * pes = cloudsystem_getPEs(_self);
	for (int i = 0; i < n; ++i) {
		pe * host = vector_get(pes, i);
		resource * r = pe_getresource(host);
		if (r) {
			float * util = resource_getutilization(r);
			for (int j = 0; j < m; ++j) {
				u[j][i] = util[j];
			}
			free(util);
		}
	}

	for (int j = 0; j < m; ++j) {
		printf("nodeUtil(%d): ", j);
		for (int i = 0; i < n; ++i) {
			printf("%f ", u[j][i]);
		}
		printf("\n");
	}

	for (int j = 0; j < m; ++j) {
		qsort(u[j], n, sizeof(float), mathhelper_floatcomp);
	}
	matrix_add(_self->orderedutil, sysM);
	_self->orderedcount++;
	float** z = matrix_getdata(_self->orderedutil);
	for (int j = 0; j < m; ++j) {
		printf("orderedStatsNodeUtil(%d): ", j);
		for (int i = 0; i < n; ++i) {
			printf("%f ", z[j][i] / _self->orderedcount);
		}
		printf("\n");
	}

	matrix_destroy(sysM);
}

void cloudsystem_displayutilization(cloudsystem * _self, bool _issorted) {
	printf("CloudSystem resource usage:\n");
	vector * v = _self->pes;
	if (v && vector_getsize(v) > 0) {
		int n = vector_getsize(v);
		char** allpes = vector_toarray(v);
		if (_issorted) {
			allpes = vector_toarraycopy(v);
			qsort(allpes, n, sizeof(char*), pe_comparePEs);
		}
		for (int i = 0; i < n; ++i) {
			pe* host = (pe*) allpes[i];
			if (host) {
				resource* r = pe_getresource(host);
				int u = resource_getusage(r)[0];
				int c = resource_getcapacity(r)[0];
				int len = c;
				if (u > c) {
					len = u;
				}
				len++;	// for the separator char

//				if (u == 0) {
//					continue;
//				}

				char* prefix = pe_getname(host);
				char* seq = (char*) calloc(len+1 , sizeof(char));
				assert(seq);
				for (int j = 0; j < u; ++j) {
					seq[j] = 'X';
				}
				for (int j = u; j < len; ++j) {
					seq[j] = ' ';
				}
				seq[len] = '\0';
				seq[c] = '|';
				if (u > c) {
					seq[u] = 'X';
				}

				char* line = (char*) malloc(
						(strlen(prefix) + len + 4) * sizeof(char));
				assert(line);
				sprintf(line, "%s: %s", prefix, seq);
				free(seq);

				printf("%s \n", line);
				free(line);
			}
		}
		if (_issorted) {
			free(allpes);
		}
	}
	printf("\n");
}

void cloudsystem_displaysummary(cloudsystem * _self, bool _issorted) {
	printf("CloudSystem summary:\n");

	char** alllicenseids = NULL;
	int numlics = 0;

	printf("pe \t occup \t cap \t usg \t");
	for (int l = 0; l < numlics; ++l) {
		printf("%s\t", alllicenseids[l]);
	}
	printf("\n");

	vector * v = _self->pes;
	if (v && vector_getsize(v) > 0) {
		int n = vector_getsize(v);
		char** allpes = vector_toarray(v);
		if (_issorted) {
			allpes = vector_toarraycopy(v);
			qsort(allpes, n, sizeof(char*), pe_comparePEs);
		}

		for (int i = 0; i < n; ++i) {
			pe* host = (pe*) allpes[i];
			if (host) {
				char* prefix = pe_getname(host);
				printf("%s: ", prefix);

				int m = 0;
				vector* hosted = pe_gethostedLE(host);
				if (hosted) {
					m = vector_getsize(hosted);
				}
				printf("%d ", m);

				resource* r = pe_getresource(host);
				int k = resource_getnum(r);
				int* u = resource_getusage(r);
				int* c = resource_getcapacity(r);
				for (int j = 0; j < k; ++j) {
					printf("%d %d ", c[j], u[j]);
				}

				printf("\n");
			}
		}
		if (_issorted) {
			free(allpes);
		}
	}

	if (alllicenseids) {
		free(alllicenseids);
	}
	printf("\n");
}

demandstats * cloudsystem_getdemandstats(cloudsystem * _self) {
	return _self->ds;
}

void cloudsystem_setdemandstats(cloudsystem * _self, demandstats * _demandstats) {
	_self->ds = _demandstats;
}

void cloudsystem_print(cloudsystem * _self) {
	printf("CloudSystem:\n");
	vector * v = _self->pes;
	if (v) {
		vector_print(v, "PEs",
				(void (*)(void*))pe_print
				);
	}
	printf("\n");

//	cloudsystem_displayutilization(_self, false);

}
