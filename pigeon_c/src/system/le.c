/*
 * ve.c
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "le.h"
#include "pe.h"
#include "../utils/printhelper.h"
#include "../utils/stringbuffer.h"
#include "../replay/parser.h"

/**
 *  Logical Entity (LE)
 *  (extends graph node)
 */
struct s_le {
	/** unique label for this node */
	char* label;

	/** resource demands */
	int* demand;

	/** length of demand array */
	int numdemand;

	/** hosting PE */
	void* host;
};


le * le_create(char* _name, int * _demand, int _numdemand) {
	le * e = (void *) malloc(sizeof(le));
	assert(e);

	int* d = (int*) malloc(_numdemand * sizeof(int));
	assert(d);

	for (int i = 0; i < _numdemand; ++i) {
		d[i] = _demand[i];
	}

	e->label = _name;
	e->demand = d;
	e->numdemand = _numdemand;
	e->host = NULL;

	return e;
}

void le_destroy(le * _self) {
	if (_self) {

		free(_self->demand);
		_self->demand = NULL;

		_self->host = NULL;
		free(_self);
	}
}

char * le_getname(le * _self) {
	return _self->label;
}


bool le_ishosted(le * _self) {
	return _self->host != NULL;
}

void * le_gethost(le * _self) {
	return _self->host;
}

void le_sethost(le * _self, void * _pe) {
	_self->host = _pe;
}

int* le_getdemand(le * _self) {
	return _self->demand;
}

int le_getnumdemand(le * _self) {
	return _self->numdemand;
}


void le_print(le * _self) {
	printf("LE: ");
	printf("name=%s; ", le_getname(_self));

	printf("isHosted=%d; ", le_ishosted(_self));
	char* hostname = le_ishosted(_self) ? le_getname(_self->host) : "NULL";
	printf("hostname=%s; ", hostname);

	if (_self->demand) {
		printhelper_printvectorint("demand", _self->demand, _self->numdemand);
		printf("; ");
	}

	printf("\n");
}
