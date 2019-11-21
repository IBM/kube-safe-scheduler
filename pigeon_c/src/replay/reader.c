/*
 * reader.c
 *
 *  Created on: Sep 4, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "reader.h"
#include "properties.h"
#include "parser.h"
#include "../utils/printhelper.h"


/**
 * A configuration reader
 */
struct s_reader {
	/** property list */
	properties * p;

	/** policy resource index */
	int policyresourceindex;

	/** policy objective */
	objective_type policyobjective;
};


reader * reader_create(char* _fname) {
	reader * r = (void *) malloc(sizeof(reader));
	assert(r);

	r->policyresourceindex = 0;
	r->policyobjective = A_BINPACK;

	r->p = properties_create();
	bool isread = properties_readfromfile(r->p, _fname);
	if (!isread) {
		printf("failure reading properties from file %s. \n", _fname);
		return NULL;
	}

	char* key;
	char* value = (char *) malloc(PROP_MAX_VALUE_LENGTH * sizeof(char));
	assert(value);
	bool isget;

	key = "policyResourceIndex";
	isget = properties_get(r->p, key, value);
	int policyResourceIndex = isget ? parser_decode_int(value) : 0;

	key = "policyObjective";
	isget = properties_get(r->p, key, value);
	objective_type objtype =
			isget ? defs_objtypeFromstring(value) : A_BINPACK;

	r->policyresourceindex = policyResourceIndex;
	r->policyobjective = objtype;

	free(value);

	return r;
}

void reader_destroy(reader * _self) {
	if (_self) {
		properties_destroy(_self->p);
		_self->p = NULL;

		free(_self);
	}
}

int reader_getpolicyresourceindex(reader * _self) {
	return _self->policyresourceindex;
}

objective_type reader_getpolicyobjective(reader * _self) {
	return _self->policyobjective;
}

bool reader_getproperty(reader * _self, char * _key, char * _value) {
	return properties_get(_self->p, _key, _value);
}

void reader_print(reader * _self) {
	printf("all properties: \n");
	properties_print(_self->p);
	printf("\n");
}
