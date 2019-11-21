/*
 * properties.c
 *
 *  Created on: Aug 22, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "properties.h"
#include "../utils/hashmap.h"


/**
 * properties list
 */
struct s_properties {
	/** hash map of properties */
	hashmap* map;
};

void printstring(char* p) {
	printf("%s ", p);
}

properties * properties_create() {
	properties * p = (void *) malloc(sizeof(properties));
	assert(p);

	hashmap* m = hashmap_create(0);
	assert(m);
	p->map = m;

	return p;
}

void properties_destroy(properties * _self) {
	if (_self) {
		hashmap_destroy(_self->map);
		_self->map = NULL;
		free(_self);
	}
}

bool properties_readfromfile(properties * _self, char * _filename) {
	/** open file for reading */
	FILE * stream = fopen(_filename, "r");
	if (!stream) {
		return false;
	}
	char* s = (char *) malloc(PROP_MAX_LINE_LENGTH * sizeof(char));
	assert(s);

	/** read one line at a time */
	while (fgets(s, PROP_MAX_LINE_LENGTH, stream)) {
		// skip comment and empty lines
		if (s[0] == PROP_COMMENT_LINE || s[0] == '\n') {
			continue;
		}

#ifdef DEBUG
		printf("%s", s);
#endif

		char* k = (char *) malloc(PROP_MAX_KEY_LENGTH * sizeof(char));
		assert(k);
		char* v = (char *) malloc(PROP_MAX_VALUE_LENGTH * sizeof(char));
		assert(v);

		// key = value
		char* eq = strchr(s, '=');
		if (eq) {
			// get key
			*eq = '\0';
			strcpy(k, s);

			// delete trailing blanks, if any
			char* endkey = strchr(k, ' ');
			if (endkey) {
				*endkey = '\0';
			}

			// delete leading blanks, if any
			char* firstvalue = eq + 1;
			while (*firstvalue != '\0' && *firstvalue == ' ') {
				firstvalue++;
			}

			// get value
			strcpy(v, firstvalue);
			char* eol = strchr(v, '\n');
			if (eol) {
				*eol = '\0';
			}

			// insert <K, V> in map
			hashmap_insert(_self->map, v, k);
		}
	}

#ifdef DEBUG
	printf("\n");
#endif

	free(s);
	fclose(stream);
	return true;
}

bool properties_get(properties * _self, char * _key, char * _value) {
	char* v = hashmap_get(_self->map, _key);
	if (v) {
		strcpy(_value, v);
		return true;
	} else {
		return false;
	}
}

vector *  properties_getkeys(properties * _self) {
	return hashmap_getkeys(_self->map);
}

void properties_clear(properties * _self) {
	hashmap_destroy(_self->map);
	hashmap* m = hashmap_create(0);
	assert(m);
	_self->map = m;
}

void properties_print(properties * _self) {
	hashmap_print(_self->map, (void (*)(void*))printstring);
	printf("\n");
}
