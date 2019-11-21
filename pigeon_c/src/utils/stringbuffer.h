/*
 * stringbuffer.h
 *
 *  Created on: Aug 29, 2014
 *      Author: assertantawi
 */

#ifndef STRINGBUFFER_H_
#define STRINGBUFFER_H_

#include <stdbool.h>

/**
 * default initial size allocated to the buffer string
 */
#define INITIAL_SIZE	64

struct s_stringbuffer;

/**
 * A dynamic string buffer
 */
typedef struct s_stringbuffer stringbuffer;

/**
 * create a stringbuffer
 */
extern stringbuffer * stringbuffer_create();

/**
 * destroy the stringbuffer
 */
extern void stringbuffer_destroy(stringbuffer * _self);

/**
 * append a string at the end of the stringbuffer and optionally free the string
 */
extern void stringbuffer_append(stringbuffer * _self, char * _string, bool _isfree);

/**
 * get the string
 */
extern char* stringbuffer_getstring(stringbuffer * _self);

/**
 * reset the string buffer to a new string, without freeing the old string
 */
extern void stringbuffer_reset(stringbuffer * _self);


#endif /* STRINGBUFFER_H_ */
