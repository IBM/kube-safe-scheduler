/*
 * consfn.c
 *
 *  Created on: Nov 20, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "consfn.h"
#include "../system/pe.h"
#include "../system/resource.h"
#include "../utils/mathhelper.h"


/**
 * A consolidation objective function
 */
struct s_consfn{

};


consfn * consfn_create() {
	consfn * fn = (void *) malloc(sizeof(consfn));
	assert(fn);

	return fn;
}


void consfn_destroy(void * _self) {
	if (_self) {

		free(_self);
	}
}


float consfn_getsystemvalue(objfn * _f) {
	cloudsystem * cs = objfn_getcloudsystem(_f);
	if (!cs) {
		return 0;
	}
	vector * pes = cloudsystem_getPEs(cs);
	int n = vector_getsize(pes);

	float* u = (float*) malloc(n * sizeof(float));
	assert(u);

	for (int i = 0; i < n; ++i) {
		pe * host = vector_get(pes, i);
		resource * r = pe_getresource(host);
		int index = objfn_getresindex(_f);
		int cap = resource_getcapacity(r)[index];
		u[i] = (cap > 0) ? resource_getusage(r)[index] / (float) cap : 1;
	}

	double value = commonobj_unbalance(u, n, UNBALANCE_SCALED);
	free(u);
	return value;
}


