/*
 * vector.c
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vector.h"

/**
 * A vector data structure where an element is a pinter to an arbitrary object.
 * All operations are based on pointers.
 * A match of an element in the vector and an arbitrary object is done based on
 * pointer equality.
 */
struct s_vector {
	/** number of elements in the vector */
	int size;
	/** current capacity of the vector (maximum number of elements) */
	int capacity;
	/** array of elements (pointers to element contents) */
	char **data;
};

/**
 * double the current memory allocation of the vector
 */
static void vector_double_capacity_if_full(vector * _self) {
	if (_self->size >= _self->capacity) {
		_self->capacity *= 2;
		_self->data = realloc(_self->data, _self->capacity * sizeof(char *));
		assert(_self->data);
	}
}


vector * vector_create() {

	vector * v = (void *) malloc(sizeof(vector));
	assert(v);

	v->size = 0;
	v->capacity = VECTOR_INITIAL_CAPACITY;
	v->data = (char **) malloc(v->capacity * sizeof(char *));
	assert(v->data);

	return v;
}

void vector_destroy(vector * _self) {
	if (_self) {
		free(_self->data);
		_self->data = NULL;

		free(_self);
	}
}

bool vector_append(vector * _self, void * _element) {
	  vector_double_capacity_if_full(_self);
	  _self->data[_self->size++] = _element;
	  return true;
}

void vector_appendall(vector * _self, vector * _other) {
	int n = vector_getsize(_other);
	for (int i = 0; i < n; ++i) {
		void* element = vector_get(_other, i);
		if (element) {
			vector_append(_self, element);
		}
	}
}

bool vector_delete(vector * _self, void * _element) {
	bool deleted = false;
	int index = vector_getindex(_self, _element);
	if (index >= 0) {
		deleted = vector_remove(_self, index);
	}
	return deleted;
}

void * vector_get(vector * _self, int _index) {
	if (_index >= _self->size || _index < 0) {
		return NULL;
	}
	return _self->data[_index];
}

void vector_set(vector * _self, int _index, void * _element) {
	while (_index >= _self->size) {
		vector_append(_self, 0);
	}
	if (_index >= 0) {
		_self->data[_index] = _element;
	}
}

int vector_getsize(vector * _self) {
	return _self->size;
}

bool vector_isempty(vector * _self) {
	return _self->size == 0;
}

int vector_getindex(vector * _self, void * _element) {
	for (int i = 0; i < _self->size; ++i) {
		if (_self->data[i] == _element) {
			return i;
		}
	}
	return -1;
}

int vector_getindexbycompare(vector * _self, void * _element,
		int (*element_compare)(void *, void *)) {
	for (int i = 0; i < _self->size; ++i) {
		if ((*element_compare)(_self->data[i], _element) == 0) {
			return i;
		}
	}
	return -1;
}

bool vector_remove(vector * _self, int _index) {
	if (_index >= 0 && _index < _self->size) {
		for (int i = _index; i < _self->size - 1; ++i) {
			_self->data[i] = _self->data[i + 1];
		}
		_self->data[_self->size - 1] = 0;
		_self->size--;
		return true;
	}
	return false;
}

bool vector_insert(vector * _self, int _index, void * _element) {
	while (_index > _self->size) {
		vector_append(_self, 0);
	}
	if (_index >= 0) {
		vector_double_capacity_if_full(_self);
		for (int i = _self->size - 1; i >= _index; --i) {
			_self->data[i + 1] = _self->data[i];
		}
		_self->data[_index] = _element;
		_self->size++;
		return true;
	}
	return false;
}

bool vector_contains(vector * _self, void * _element) {
	return vector_getindex(_self, _element) >= 0;
}

bool vector_containsbycompare(vector * _self, void * _element,
		int (*element_compare)(void *, void *)) {
	return vector_getindexbycompare(_self, _element, element_compare) >= 0;
}

void vector_clear(vector * _self) {
	_self->size = 0;
	_self->capacity = VECTOR_INITIAL_CAPACITY;
	_self->data = realloc(_self->data, _self->capacity * sizeof(char *));
	assert(_self->data);
}

char ** vector_toarray(vector * _self) {
	return _self->data;
}

char ** vector_toarraycopy(vector * _self) {
	char ** a = (char **) malloc(_self->size * sizeof(char *));
	memcpy(a, _self->data, _self->size * sizeof(char *));
	return a;
}

void vector_print(vector * _self, char* _name, void (*element_print)(void *)) {
	if (_name == NULL || _self == NULL || (*element_print) == NULL) {
		return;
	}
	printf("%s:\n", _name);
	for (int i = 0; i < vector_getsize(_self); ++i) {
		void * ei = vector_get(_self, i);
		if (ei) {
			(*element_print)(ei);
		}
	}
}

void vector_printbrief(vector * _self, void (*element_print)(void *)) {
	if (_self == NULL || (*element_print) == NULL) {
		return;
	}
	int n = vector_getsize(_self);
	for (int i = 0; i < n; ++i) {
		void * ei = vector_get(_self, i);
		if (ei) {
			(*element_print)(ei);
			if (i < n - 1) {
				printf(",");
			}
		}
	}
}
