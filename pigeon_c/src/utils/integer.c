/*
 * integer.c
 *
 *  Created on: Jan 30, 2018
 *      Author: tantawi
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "integer.h"

/**
 * An Integer object
 */
struct s_integer {
	/** value of the object */
	int value;
};

integer* integer_create(int _value) {
	integer* i = (void*) malloc(sizeof(integer));
	assert(i);
	i->value = _value;
	return i;
}

void integer_destroy(integer* _self) {
	if (_self) {
		free(_self);
	}
}

int integer_value(integer* _self) {
	return _self->value;
}

void integer_set(integer* _self, int _value) {
	_self->value = _value;
}

int integer_compare(void* _self, void* _other) {
	int v1 = integer_value(_self);
	int v2 = integer_value(_other);
	return (v1 > v2) - (v1 < v2);
}

void integer_print(integer* _self) {
	printf("%d", _self->value);
}
