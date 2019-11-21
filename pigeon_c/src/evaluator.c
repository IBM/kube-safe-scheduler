/*
 * evaluator.c
 *
 *  Created on: Feb 27, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#include "evaluator.h"
#include "utils/vector.h"
#include "system/cloudsystem.h"
#include "system/pe.h"
#include "system/resource.h"
#include "system/le.h"
#include "system/stateupdater.h"
#include "replay/reader.h"
#include "objective/objfn.h"


/**
 * A system evaluator for scheduler
 */
struct s_evaluator {
	/** the cloud system model */
	cloudsystem * cs;

	/** the model state updater */
	stateupdater * updater;

	/** policy resource index */
	int policyresourceindex;

	/** policy objective */
	objective_type policyobjective;
};


evaluator * evaluator_create(int _policyresourceindex, objective_type _policyobjective) {

	evaluator * eval = (void *) malloc(sizeof(evaluator));
	assert(eval);

	eval->policyresourceindex = _policyresourceindex;
	eval->policyobjective = _policyobjective;

	vector* pes = vector_create();
	cloudsystem* cs = cloudsystem_create(pes);
	eval->cs = cs;

	eval->updater = stateupdater_create(cs);

	if (_policyobjective == A_BINPACK) {
		demandstats* dstats = demandstats_create(0);
		cloudsystem_setdemandstats(cs, dstats);
	}

	return eval;
}


evaluator * evaluator_createfromfile(char* _fname) {
	int policyresourceindex = 0;
	objective_type policyobjective = A_BINPACK;

	reader* r = reader_create(_fname);
	if (r) {
		policyresourceindex = reader_getpolicyresourceindex(r);
		policyobjective = reader_getpolicyobjective(r);
		reader_destroy(r);
	}

#ifdef DEBUG
	else {
		printf("Could not read configuration file, using default values \n");
	}
#endif

	return evaluator_create(policyresourceindex, policyobjective);
}


void evaluator_destroy(evaluator * _self) {
	if (_self) {

		if (_self->cs) {
			cloudsystem_destroy(_self->cs);
			_self->cs = NULL;
		}

		if (_self->updater) {
			stateupdater_destroy(_self->updater);
			_self->updater = NULL;
		}

		free(_self);
	}
}

float* evaluator_eval(evaluator * _self, void* _le) {
	cloudsystem * cs = _self->cs;

	if (!cs || !_le) {
		return NULL;
	}

	demandstats* dstats = cloudsystem_getdemandstats(cs);
	if (dstats) {
		demandstats_add(dstats, _le);
	}

	objfn * fn = objfn_create(cs, _le, _self->policyobjective,
			_self->policyresourceindex);

#ifdef DEBUG
	objfn_setprint(fn, true);
#endif

	vector* hosts = cloudsystem_getPEs(cs);
	int n = vector_getsize(hosts);

	float* val = (float*) malloc(n * sizeof(float));
	assert(val);

	for (int i = 0; i < n; ++i) {
		pe* pei = vector_get(hosts, i);
		if (pei && pe_canplaceLE(pei, _le)) {
			pe_placeLE(pei, _le, true);


#ifdef DEBUG
			char* peiname = pe_getname(pei);
			printf("Node %s:", peiname);
#endif

			val[i] = objfn_getvalue(fn);
			pe_removeLE(pei, _le, true);
		} else {
			val[i] = NAN;
		}
	}

	objfn_destroy(fn);

	return val;
}

void * evaluator_getstateupdater(evaluator * _self) {
	return _self->updater;
}

int evaluator_getnumresources(evaluator * _self) {
	cloudsystem* cs = _self->cs;
	int n = 0;
	if (cs) {
		n = cloudsystem_getnumresources(cs);
	}
	return n;
}

char* evaluator_getpolicyobjective(evaluator * _self) {
	return defs_objtypeTostring(_self->policyobjective);
}

void evaluator_setpolicyobjective(evaluator * _self, char* _policyobjective) {
	_self->policyobjective = defs_objtypeFromstring(_policyobjective);
}


void evaluator_print(evaluator * _self) {
	cloudsystem* cs = _self->cs;
	if (cs) {
		cloudsystem_print(cs);
	}
}

