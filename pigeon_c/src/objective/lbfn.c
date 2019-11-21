/*
 * lbfn.c
 *
 *  Created on: Nov 13, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "lbfn.h"
#include "../system/pe.h"
#include "../system/resource.h"
#include "../utils/mathhelper.h"


/**
 * A load balancing objective function
 */
struct s_lbfn{

};


lbfn * lbfn_create() {
	lbfn * fn = (void *) malloc(sizeof(lbfn));
	assert(fn);

	return fn;
}


void lbfn_destroy(void * _self) {
	if (_self) {

		free(_self);
	}
}


float lbfn_getsystemvalue(objfn * _f) {
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

	double value = commonobj_balance(u, n);
	free(u);
	return value;
}
