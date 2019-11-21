/*
 * stateupdater.c
 *
 *  Created on: May 31, 2018
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "stateupdater.h"

#include "pe.h"
#include "../utils/hashset.h"
#include "../utils/integer.h"

/**
 * An updater to the state of the cloud system model given the observed state
 * System model is assumed to be already built (could be empty)
 *
 * Updates are communicated in a session delimited by start() and stop().
 * Observations about all PEs and all LEs in the system are to be provided
 * by update(). The order is important: update PEs first then LEs.
 * start() and stop() should be called only once each in a session.
 *
 * When updating an LE host placement change, all LE removals are done first,
 * then all LE placements are done in the end of the update session.
 * This does not result in overflowing resources while effecting the changes.
 */
struct s_stateupdater {
	/** the cloud system model */
	cloudsystem* cs;

	/** generation number of last update session */
	long gen;

	/** indicator that an update session has started */
	bool started;

	/** set of all PEs observed (peID) */
	hashset* observedPEs;

	/** set of all LEs observed (leID) */
	hashset* observedLEs;

	/** map of all LEs in model (leID -> le) */
	hashmap* lemap;

	/** map of new placements (leID -> peID) */
	hashmap* newplacements;

	/** start time of update (for reporting) */
	clock_t starttime;

	/** end time of update (for reporting) */
	clock_t stoptime;

	/** PEs added during update (for reporting) */
	vector* addedPEs;

	/** PEs added during update (for reporting) */
	vector* deletedPEs;

	/** PEs added during update (for reporting) */
	vector* updatedPEs;

	/** LEs added during update (for reporting) */
	vector* addedLEs;

	/** LEs added during update (for reporting) */
	vector* deletedLEs;

	/** LEs added during update (for reporting) */
	vector* updatedLEs;
};

stateupdater * stateupdater_create(cloudsystem* _cs) {
	stateupdater * su = (void *) malloc(sizeof(stateupdater));
	assert(su);

	su->cs = _cs;
	su->gen = -1;
	su->started = false;

	su->observedPEs = NULL;
	su->observedLEs = NULL;

	su->lemap = NULL;
	su->newplacements = NULL;

	su->starttime = clock();
	su->stoptime = clock();

	su->addedPEs = vector_create();
	su->deletedPEs = vector_create();
	su->updatedPEs = vector_create();

	su->addedLEs = vector_create();
	su->deletedLEs = vector_create();
	su->updatedLEs = vector_create();

	return su;
}

void stateupdater_destroy(stateupdater* _self) {
	if (_self) {

		if (_self->observedPEs) {
			hashset_destroy(_self->observedPEs);
			_self->observedPEs = NULL;
		}
		if (_self->observedLEs) {
			hashset_destroy(_self->observedLEs);
			_self->observedLEs = NULL;
		}

		if (_self->lemap) {
			hashmap_destroy(_self->lemap);
			_self->lemap = NULL;
		}
		if (_self->newplacements) {
			hashmap_destroy(_self->newplacements);
			_self->newplacements = NULL;
		}

		if(_self->addedPEs) {
			vector_destroy(_self->addedPEs);
			_self->addedPEs = NULL;
		}
		if(_self->deletedPEs) {
			vector_destroy(_self->deletedPEs);
			_self->deletedPEs = NULL;
		}
		if(_self->updatedPEs) {
			vector_destroy(_self->updatedPEs);
			_self->updatedPEs = NULL;
		}

		if(_self->addedLEs) {
			vector_destroy(_self->addedLEs);
			_self->addedLEs = NULL;
		}
		if(_self->deletedLEs) {
			vector_destroy(_self->deletedLEs);
			_self->deletedLEs = NULL;
		}
		if(_self->updatedLEs) {
			vector_destroy(_self->updatedLEs);
			_self->updatedLEs = NULL;
		}

		free(_self);
	}
}

bool stateupdater_start(stateupdater* _self, long _generation) {

	if (_generation <= _self->gen) {
		_self->started = false;
		return false;
	}

	// close prior session if not closed
	if (_self->started) {
		stateupdater_stop(_self);
	}

	// start new session
	_self->starttime = clock();
	_self->started = true;
	_self->gen = _generation;

	vector_clear(_self->addedPEs);
	vector_clear(_self->deletedPEs);
	vector_clear(_self->updatedPEs);

	vector_clear(_self->addedLEs);
	vector_clear(_self->deletedLEs);
	vector_clear(_self->updatedLEs);

	// initialize
	_self->observedPEs = hashset_create(0);
	_self->observedLEs = hashset_create(0);

	_self->lemap = hashmap_create(0);
	_self->newplacements = hashmap_create(0);

	// fill LE and pattern maps from model
	vector* allpes = cloudsystem_getPEs(_self->cs);
	for (int i = 0; i < vector_getsize(allpes); ++i) {
		pe* pei = vector_get(allpes, i);
		vector* les = pe_gethostedLE(pei);
		for (int j = 0; j < vector_getsize(les); ++j) {
			le* lej = vector_get(les, j);
			hashmap_insert(_self->lemap, lej, le_getname(lej));
		}
	}

	return true;
}

void stateupdater_stop(stateupdater* _self) {
	if (!_self->started) {
		return;
	}

//	printf("\n\t ==> STATEUPDATER: state before updating ... \n");
//	cloudsystem_print(_self->cs);

	// handle PEs
	int numpes = cloudsystem_getnumPEs(_self->cs);
	pe** allpes = (pe**) vector_toarraycopy(cloudsystem_getPEs(_self->cs));
	for (int i = 0; i < numpes; ++i) {
		pe* pei = allpes[i];
		char* idi = pe_getname(pei);
		if (!hashset_find(_self->observedPEs, idi)) {
			cloudsystem_remove_PE(_self->cs, idi);
			vector_append(_self->deletedPEs, idi);
		}
	}
	free(allpes);
	cloudsystem_resetpes(_self->cs);

	// handle deleted LEs
	vector* allles = hashmap_getvalues(_self->lemap);
	for (int i = 0; i < vector_getsize(allles); ++i) {
		le* lei = vector_get(allles, i);
		if (!hashset_find(_self->observedLEs, le_getname(lei))) {
			pe* pei = le_gethost(lei);
			if (pei) {
				pe_removeLE(pei, lei, true);
				vector_append(_self->deletedLEs, le_getname(lei));
			}
		}
	}
	vector_destroy(allles);

	// handle new placements
	vector* leids = hashmap_getkeys(_self->newplacements);
	for (int i = 0; i < vector_getsize(leids); ++i) {
		char* leID = vector_get(leids, i);
		le* lei = hashmap_get(_self->lemap, leID);
		if (lei) {
			char* peiID = hashmap_get(_self->newplacements, leID);
			pe* pei = cloudsystem_getPE(_self->cs, peiID);
			if (pei) {
				pe_placeLE(pei, lei, true);
			}
		}
	}
	vector_destroy(leids);


//	printf("\n\t ==> STATEUPDATER: state after updating ... \n");
//	cloudsystem_print(_self->cs);

	// cleanup
	hashset_destroy(_self->observedPEs);
	_self->observedPEs = NULL;
	hashset_destroy(_self->observedLEs);
	_self->observedLEs = NULL;
	hashmap_destroy(_self->lemap);
	_self->lemap = NULL;
	hashmap_destroy(_self->newplacements);
	_self->newplacements = NULL;

	// close session
	_self->started = false;
	_self->stoptime = clock();
}

void stateupdater_updatePE(stateupdater* _self, char* _id, int* _capacity,
		bool* _ovf, int* _usage, int _length) {

	if (!_self->started) {
		return;
	}

	hashset_add(_self->observedPEs, _id);

	pe* pex = cloudsystem_getPE(_self->cs, _id);
	if (pex) {

		/*
		 * PE already exists
		 * assumptions: overflow and number of resources do not change
		 */
		bool haschanged = false;
		int* currentCap = resource_getcapacity(pe_getresource(pex));
		int* currentUse = resource_getusage(pe_getresource(pex));
		for (int k = 0; k < _length; ++k) {
			haschanged |= (currentCap[k] != _capacity[k]);
			haschanged |= _usage && currentUse[k] != _usage[k];
			if (haschanged) {
				pe_update(pex, _capacity, _usage);
				vector_append(_self->updatedPEs, _id);
				break;
			}
		}
	} else {

		/*
		 * new PE
		 */
		resource* res = resource_create(_length, _capacity, _ovf);
		if (_usage) {
			resource_setusage(res, _usage);
		}
		pe* penew = pe_create(_id, res);
		cloudsystem_addPE(_self->cs, penew);
		vector_append(_self->addedPEs, _id);
	}
}

void stateupdater_updateLE(stateupdater* _self, char* _id,
		char* _hostID, int* _demand, int _length) {

	if (!_self->started) {
		return;
	}

	hashset_add(_self->observedLEs, _id);

	le* lex = hashmap_get(_self->lemap, _id);
	if (lex) {

		/*
		 * LE already exists
		 * assumptions: pattern, resource demand, and number of resources do not change
		 */
		pe* pex = le_gethost(lex);
		if (pex) {

			// check if host has changed
			if (strcmp(pe_getname(pex), _hostID) == 0) {
				return;
			} else {
				pe_removeLE(pex, lex, true);
				vector_append(_self->updatedLEs, _id);
			}
		}
	} else {

		/*
		 * new LE
		 */
		lex = le_create(_id, _demand, _length);
		vector_append(_self->addedLEs, _id);
		hashmap_insert(_self->lemap, lex, _id);
	}

	hashmap_insert(_self->newplacements, _hostID, _id);
}

cloudsystem* stateupdater_getupdated(stateupdater* _self) {
	return _self->cs;
}

void stateupdater_printid(char* _id) {
	printf("%s", _id);
}

void stateupdater_print(stateupdater * _self) {
	printf("stateupdater: ");

	int updateTime = 1000* ((double) (_self->stoptime - _self->starttime)) / CLOCKS_PER_SEC;
	printf("updateTime=%dms; ", updateTime);
	printf("\n");

	printf("addedPEs:[");
	vector_printbrief(_self->addedPEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("deletedPEs:[");
	vector_printbrief(_self->deletedPEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("updatedPEs:[");
	vector_printbrief(_self->updatedPEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("addedLEs:[");
	vector_printbrief(_self->addedLEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("deletedLEs:[");
	vector_printbrief(_self->deletedLEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("updatedLEs:[");
	vector_printbrief(_self->updatedLEs, (void (*)(void*))stateupdater_printid);
	printf("] \n");

	printf("\n");
}
