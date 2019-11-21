/*
 * pe.c
 *
 *  Created on: Feb 8, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "pe.h"
#include "../utils/printhelper.h"
#include "../utils/stringbuffer.h"
#include "../utils/hashmap.h"
#include "../replay/parser.h"

/**
 * Physical Entity (PE)
 */
struct s_pe {
	/** unique label for this node */
	char* label;

	/** resources held by PE */
	resource * resource;

	/** list of LEs hosted by this PE */
	vector * hostedLE;

	/** index of PE in cloud system */
	int indexinsystem;
};


pe * pe_create(char* _name, resource * _resource) {
	pe * p = (void *) malloc(sizeof(pe));
	assert(p);

	p->label = _name;
	p->resource = _resource;
	p->hostedLE = vector_create();
	p->indexinsystem = -1;

	return p;
}


void pe_destroy(pe* _self) {
	if (_self) {

		if (_self->resource) {
			resource_destroy(_self->resource);
			_self->resource = NULL;
		}

		if (_self->hostedLE) {
			for (int i = 0; i < vector_getsize(_self->hostedLE); ++i) {
				le * v = vector_get(_self->hostedLE, i);
				le_destroy(v);
			}

			vector_destroy(_self->hostedLE);
			_self->hostedLE = NULL;
		}

		free(_self);
	}
}


char * pe_getname(pe * _self) {
	return _self->label;
}

/**
 * Taking two pointers as arguments (both converted to const void*).
 * The function defines the order of the elements by returning
 * (in a stable and transitive manner):
 *
 * return					meaning
 * 	<0		The element pointed by p1 goes before the element pointed by p2
 * 	0		The element pointed by p1 is equivalent to the element pointed by p2
 * 	>0		The element pointed by p1 goes after the element pointed by p2
 *
 */
int pe_comparePEs(const void * _pe1, const void * _pe2) {
	pe ** pe1 = (pe **) _pe1;
	pe ** pe2 = (pe **) _pe2;
	return strcmp(pe_getname(*pe1), pe_getname(*pe2));
}

bool pe_canplaceLE(pe* _self, le* _le) {
	if (!resource_canuse(_self->resource, _le)) {
		return false;
	}

	return true;
}


bool pe_placeLE(pe* _self, le* _le, bool _update_resource) {
	if (vector_contains(_self->hostedLE, _le)) {
		return true;
	}

	bool normalLoad =
			(_update_resource) ?
					resource_use(_self->resource, _le) :
					resource_isnormalload(_self->resource);

	vector_append(_self->hostedLE, _le);
	le_sethost(_le, _self);
	return normalLoad;
}


bool pe_removeLE(pe* _self, le* _le, bool _update_resource) {
	int index = vector_getindex(_self->hostedLE, _le);
	if (index < 0) {
		return true;
	}

	bool normalLoad =
			(_update_resource) ?
					resource_release(_self->resource, _le) :
					resource_isnormalload(_self->resource);

	vector_remove(_self->hostedLE, index);
	le_sethost(_le, NULL);
	return normalLoad;
}


vector * pe_gethostedLE(pe* _self) {
	return _self->hostedLE;
}

void pe_clear(pe* _self, bool _update_resource) {
	vector* temp = vector_create();
	vector_appendall(temp, _self->hostedLE);
	for (int i = 0; i < vector_getsize(temp); i++) {
		le* lei = vector_get(temp, i);
		pe_removeLE(_self, lei, _update_resource);
	}
	vector_destroy(temp);
}


resource * pe_getresource(pe* _self) {
	return _self->resource;
}


void pe_update(pe* _self, int* _capacity, int* _usage) {
	if (_capacity) {
		resource_setcapacity(_self->resource, _capacity);
	}
	if (_usage) {
		resource_setusage(_self->resource, _usage);
	}
}


float pe_getprimeutilization(pe* _self, int _res_index) {
	float pu = 0;
	if (_self->resource) {
		float* u = resource_getutilization(_self->resource);
		pu = u[_res_index];
		free(u);
	}
	return pu;
}

int pe_getindexinsystem(pe * _self) {
	return _self->indexinsystem;
}

void pe_setindexinsystem(pe * _self, int _indexinsystem) {
	_self->indexinsystem = _indexinsystem;
}

void pe_print(pe * _self) {
	printf("PE: ");
	printf("name=%s; ", pe_getname(_self));
	printf("idx=%d; ", _self->indexinsystem);

	resource * r = _self->resource;
	if (r) {
		resource_print(r);
	}

	// hosted LEs
	vector* v = _self->hostedLE;
	if (v) {
		int num = vector_getsize(v);
		printf("hostedLEs=[");
		for (int i = 0; i < num; ++i) {
			le * e = vector_get(v, i);
			if (e) {
				printf("%s", le_getname(e));
			}
			if (i < num - 1)
				printf(",");
		}
		printf("]");
		printf("; ");
	}

	printf("\n");
}
