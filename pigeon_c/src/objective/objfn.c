/*
 * objfn.c
 *
 *  Created on: Nov 13, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "objfn.h"
#include "lbfn.h"
#include "consfn.h"
#include "abpfn.h"


/**
 * The objective function to be optimized
 */
struct s_objfn {
	/** the cloud system */
	cloudsystem * cs;

	/** the LE */
	le * le;

	/** policy resource index */
	int resindex;

	/* reference to specific objective function object */
	void * func;

	/* print option for debugging */
	bool isprint;

	/** abstract function:
	 * get the value of the system component of the objective function
	 * for the implementation of the specific function func */
	float (*getsystemvalue)(objfn * _self);

	/** abstract function:
	 * destroy the specific function func */
	void (*destroy)(void * _func);
};


objfn * objfn_create(cloudsystem * _cs, le * _le, objective_type _type, int _primeresourceindex) {
	objfn * f = (void *) malloc(sizeof(objfn));
	assert(f);

	f->cs = _cs;
	f->le = _le;
	f->resindex = _primeresourceindex;

	f->isprint = false;

	switch (_type) {
	case LOAD_BALANCE:
		f->func = lbfn_create();
		f->getsystemvalue = (float (*)(objfn*)) lbfn_getsystemvalue;
		f->destroy = (void (*)(void*)) lbfn_destroy;
		break;
	case CONSOLIDATE:
		f->func = consfn_create();
		f->getsystemvalue = (float (*)(objfn*)) consfn_getsystemvalue;
		f->destroy = (void (*)(void*)) consfn_destroy;
		break;
	case A_BINPACK:
		f->func = abpfn_create();
		f->getsystemvalue = (float (*)(objfn*)) abpfn_getsystemvalue;
		f->destroy = (void (*)(void*)) abpfn_destroy;
		break;
	default:
		// A_BINPACK:
		f->func = abpfn_create();
		f->getsystemvalue = (float (*)(objfn*)) abpfn_getsystemvalue;
		f->destroy = (void (*)(void*)) abpfn_destroy;
	}

	return f;
}


void objfn_destroy(objfn * _self) {
	if (_self) {
		if (_self->func){
			(*_self->destroy)(_self->func);
		}
		_self->func = NULL;
		free(_self);
	}
}


float objfn_getvalue(objfn * _self) {
	if (_self->func) {
		float systemvalue = (*_self->getsystemvalue)(_self);
		return systemvalue;
	} else {
		return 0;
	}
}

float objfn_getsystemvalue(objfn * _self) {
	return (*_self->getsystemvalue)(_self);
}

void * objfn_getbasefunc(objfn * _self) {
	return _self->func;
}

void objfn_setprint(objfn * _self, bool _isprint) {
	_self->isprint = _isprint;
}

bool objfn_isprint(objfn * _self) {
	return _self->isprint;
}

cloudsystem * objfn_getcloudsystem(objfn * _self) {
	return _self->cs;
}

int objfn_getresindex(objfn * _self) {
	return _self->resindex;
}
