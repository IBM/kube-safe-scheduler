/*
 * stringbuffer.c
 *
 *  Created on: Aug 29, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stringbuffer.h"


struct s_stringbuffer {
	/** the string */
	char * s;

	/** buffer size allocated to the string */
	int bufsize;
};

/**
 * double buffer size until the string buffer appended with a string can fit
 */
void stringbuffer_extend_if_short(stringbuffer * _self, char* _string) {
	int desiredlength = strlen(_self->s) + strlen(_string);
	if (desiredlength >= _self->bufsize) {
		_self->bufsize *= 2;
		while (desiredlength >= _self->bufsize) {
			_self->bufsize *= 2;
		}
		_self->s = realloc(_self->s, _self->bufsize * sizeof(char));
		assert(_self->s);
	}
}

stringbuffer * stringbuffer_create() {
	stringbuffer * sb = (void *) malloc(sizeof(stringbuffer));
	assert(sb);

	sb->s = (char*) malloc(INITIAL_SIZE * sizeof(char));
	assert(sb->s);
	sb->bufsize = INITIAL_SIZE;

	sb->s[0] = '\0';
	return sb;
}

void stringbuffer_destroy(stringbuffer * _self) {
	if (_self) {
		if (_self->s) {
			free(_self->s);
			_self->s = NULL;
		}
		free(_self);
	}
}

void stringbuffer_append(stringbuffer * _self, char * _string, bool _isfree) {
	stringbuffer_extend_if_short(_self, _string);
	strcat(_self->s, _string);
	if (_isfree) {
		free(_string);
	}
}

char* stringbuffer_getstring(stringbuffer * _self) {
	return _self->s;
}

void stringbuffer_reset(stringbuffer * _self) {
	_self->s = (char*) malloc(INITIAL_SIZE * sizeof(char));
	assert(_self->s);
	_self->bufsize = INITIAL_SIZE;
	_self->s[0] = '\0';
}
