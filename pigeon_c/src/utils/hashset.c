/*
 * hashset.c
 *
 *  Created on: May 31, 2018
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hashset.h"

/**
 * A set of char* members backed by a hash map
 */
struct s_hashset {
	/** the hash map behind the set */
	hashmap* map;
};

hashset* hashset_create(int _startsize) {
	hashset * set = (void *) malloc(sizeof(hashset));
	assert(set);

	set->map = hashmap_create(_startsize);

	return set;
}

void hashset_destroy(hashset* _self) {
	if (_self) {
		if (_self->map) {
			hashmap_destroy(_self->map);
		}
		free(_self);
	}
}

void hashset_add(hashset* _self, char* _elem) {
	hashmap_insert(_self->map, _self, _elem);
}

bool hashset_delete(hashset* _self, char* _elem) {
	return !(hashmap_remove(_self->map, _elem) == NULL);
}

bool hashset_find(hashset* _self, char* _elem) {
	return !(hashmap_get(_self->map, _elem) == NULL);
}

int hashset_count(hashset* _self) {
	return (int) hashmap_count(_self->map);
}

vector* hashset_getelements(hashset* _self) {
	return hashmap_getkeys(_self->map);
}

void hashset_print(hashset* _self) {
	printf("{");
	vector* elements = hashset_getelements(_self);
	int num = vector_getsize(elements);
	for (int i = 0; i < num; ++i) {
		char* elem = vector_get(elements, i);
		printf("%s", elem);
		if (i < num - 1) {
			printf(", ");
		}
	}
	vector_destroy(elements);
	printf("}");
}
