/*
 * maparray.c
 *
 *  Created on: Jul 21, 2015
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hashmap.h"
#include "maparray.h"


/**
 * Given two immutable, associated arrays of same length, one of pointers to key values,
 * and the other of primitive typed values (e.g. int, float, etc), maparray provides a
 *  map lookup functionality: key=>value.
 */
struct s_maparray {
	/** number of elements */
	int size;

	/** array of keys */
	char** keyarray;

	/** array of values */
	void* valuearray;

	/** hashmap key=>value */
	hashmap* map;
};


maparray * maparray_create(int _arraysize, char** _keyarray,
		void* _valuearray, int _valuelength) {

	maparray * m = (void *) malloc(sizeof(maparray));
	assert(m);

	m->size = _arraysize;
	m->keyarray = _keyarray;
	m->valuearray = _valuearray;
	m->map = hashmap_create(0);

	void* ptr = _valuearray;
	for (int i = 0; i < _arraysize; ++i) {
		hashmap_insert(m->map, ptr, _keyarray[i]);
		ptr += _valuelength;
	}

	return m;
}

void maparray_destroy(maparray * _self) {
	if (_self) {
		if (_self->map) {
			hashmap_destroy(_self->map);
			_self->map = NULL;
		}
		_self->size = 0;
		free(_self);
	}
}

void* maparray_get(maparray* _self, char* _key) {
	return hashmap_get(_self->map, _key);
}

int maparray_size(maparray* _self) {
	return _self->size;
}

char** maparray_keys(maparray* _self) {
	return _self->keyarray;
}

void* maparray_values(maparray* _self) {
	return _self->valuearray;
}

void maparray_print(maparray * _self, void (*value_print)(void *)) {
	hashmap_print(_self->map, value_print);
}
