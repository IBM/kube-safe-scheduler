/*
 * resource.c
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "resource.h"
#include "../utils/printhelper.h"
#include "../utils/stringbuffer.h"
#include "../replay/parser.h"

/**
 * Resources of different types held by a PE
 */
struct s_resource {
	/** number of resource types */
	int num;

	/** total resource capacity (used and unused) */
	int* capacity;

	/** amount used of resource capacity (actual not fraction) */
	int* usage;

	/** allowing allocation overflow */
	bool* allowoverflow;
};


resource * resource_create(int _num, int* _capacity, bool* _allowoverflow) {
	resource * r = (void *) malloc(sizeof(resource));
	assert(r);

	int* c = (int*) malloc(_num * sizeof(int));
	assert(c);

	bool* ovf = (bool*) malloc(_num * sizeof(bool));
	assert(ovf);

	for (int i = 0; i < _num; ++i) {
		c[i] = _capacity[i];
		ovf[i] = (_allowoverflow) ? _allowoverflow[i] : false;
	}

	int* u = (int*) calloc(_num, sizeof(int));
	assert(u);

	r->num = _num;
	r->capacity = c;
	r->usage = u;
	r->allowoverflow = ovf;

	return r;
}


void resource_destroy(resource * _self) {
	if (_self) {

		if (_self->capacity) {
			free(_self->capacity);
			_self->capacity = NULL;
		}

		if (_self->usage) {
			free(_self->usage);
			_self->usage = NULL;
		}

		if (_self->allowoverflow) {
			free(_self->allowoverflow);
			_self->allowoverflow = NULL;
		}

		free(_self);
	}
}


bool resource_canuse(resource * _self, le * _le) {
	int* demand = le_getdemand(_le);
	bool canUse = true;
	for (int i = 0; i < _self->num; i++) {
		canUse &= _self->allowoverflow[i]
				|| (demand[i] <= (_self->capacity[i] - _self->usage[i]));
	}
	return canUse;
}


bool resource_use(resource * _self, le * _le) {
	int* demand = le_getdemand(_le);
	bool normalLoad = true;
	for (int i = 0; i < _self->num; i++) {
		_self->usage[i] += demand[i];
		if (_self->usage[i] > _self->capacity[i]) {
			normalLoad = false;
		}
	}
	return normalLoad;
}


bool resource_release(resource * _self, le * _le) {
	int* demand = le_getdemand(_le);
	bool normalLoad = true;
	for (int i = 0; i < _self->num; i++) {
		_self->usage[i] -= demand[i];
		if (_self->usage[i] < 0) {
			_self->usage[i] = 0;
			normalLoad = false;
		}
	}
	return normalLoad;
}

int resource_getnum(resource * _self) {
	return _self->num;
}

float* resource_getutilization(resource * _self) {
	float* u = (float*) malloc(_self->num * sizeof(float));
	assert(u);

	for (int i = 0; i < _self->num; i++) {
		if (_self->capacity[i] == 0) {
			u[i] = (_self->usage[i] == 0) ? 0 : 1;
		} else {
			u[i] = ((float) _self->usage[i]) / ((float) _self->capacity[i]);
			u[i] = (u[i] > 1) ? 1 : u[i];
			u[i] = (u[i] < 0) ? 0 : u[i];
		}
	}
	return u;
}

float* resource_getoverutilization(resource * _self) {
	float* ou = (float*) malloc(_self->num * sizeof(float));
	assert(ou);

	for (int i = 0; i < _self->num; i++) {
		if (_self->capacity[i] == 0) {
			ou[i] = (_self->usage[i] == 0) ? 0 : 1;
		} else {
			float u = ((float) _self->usage[i]) / ((float) _self->capacity[i]);
			ou[i] = (u > 1) ? u - 1 : 0;
		}
	}
	return ou;
}

int* resource_getcapacity(resource * _self) {
	return _self->capacity;
}

void resource_setcapacity(resource * _self, int* _capacity) {
	for (int i = 0; i < _self->num; i++) {
		_self->capacity[i] = (_capacity[i] >= 0) ? _capacity[i] : 0;
	}
}

int* resource_getusage(resource * _self) {
	return _self->usage;
}

void resource_setusage(resource * _self, int* _usage) {
	for (int i = 0; i < _self->num; i++) {
		_self->usage[i] = (_usage[i] >= 0) ? _usage[i] : 0;
	}
}

void resource_incrementusage(resource * _self, int* _deltausage) {
	for (int i = 0; i < _self->num; i++) {
		_self->usage[i] += _deltausage[i];
		if (_self->usage[i] < 0) {
			_self->usage[i] = 0;
		}
	}
}

int* resource_getavailability(resource * _self) {
	int* a = (int*) malloc(_self->num * sizeof(int));
	assert(a);

	for (int i = 0; i < _self->num; i++) {
		int avail = _self->capacity[i] - _self->usage[i];
		a[i] = (avail >= 0) ? avail : 0;
	}
	return a;
}

bool resource_isnormalload(resource * _self) {
	bool normalLoad = true;
	for (int i = 0; i < _self->num; i++) {
		if (_self->usage[i] > _self->capacity[i]) {
			normalLoad = false;
		}
	}
	return normalLoad;
}

bool* resource_allowoverflow(resource * _self) {
	return _self->allowoverflow;
}

void resource_print(resource * _self) {
	int n = resource_getnum(_self);

	int* c = resource_getcapacity(_self);
	int* u = resource_getusage(_self);
	bool* ovf = _self->allowoverflow;

	printhelper_printvectorint("resCap", c, n);
	printf("; ");

	printhelper_printvectorint("resUsed", u, n);
	printf("; ");

	printhelper_printvectorbool("ovf", ovf, n);
	printf("; ");

}
