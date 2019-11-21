/*
 * maparray.h
 *
 *  Created on: Jul 21, 2015
 *      Author: assertantawi
 */

#ifndef UTILS_MAPARRAY_H_
#define UTILS_MAPARRAY_H_

#include <stdbool.h>

struct s_maparray;

/**
 * Given two immutable, associated arrays of same length, one of pointers to key values,
 * and the other of primitive typed values (e.g. int, float, etc), maparray provides a
 *  map lookup functionality: key=>value.
 */
typedef struct s_maparray maparray;

/**
 * create a maparray
 * (array arguments are not copied)
 */
extern maparray* maparray_create(int _arraysize, char** _keyarray,
		void* _valuearray, int _valuelength);

/**
 * destroy the maparray
 */
extern void maparray_destroy(maparray * _self);

/**
 * return the value associated with a key (NULL if not found)
 */
extern void* maparray_get(maparray* _self, char* _key);

/**
 * the number of elements in the array
 */
extern int maparray_size(maparray* _self);

/**
 * the array of keys
 */
extern char** maparray_keys(maparray* _self);

/**
 * the array of values
 */
extern void* maparray_values(maparray* _self);

/**
 * toString()
 * provide function for printing the values
 */
extern void maparray_print(maparray * _self, void (*value_print)(void *));


#endif /* UTILS_MAPARRAY_H_ */
