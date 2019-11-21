/*
 * vector.h
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>

/**
 * initial capacity (number of elements) of the vector
 */
#define VECTOR_INITIAL_CAPACITY 10

struct s_vector;

/**
 * A vector data structure where an element is a pinter to an arbitrary object.
 * All operations are based on pointers.
 * A match of an element in the vector and an arbitrary object is done based on
 * pointer equality.
 */
typedef struct s_vector vector;

/**
 * create a vector
 */
extern vector * vector_create();

/**
 * destroy the vector
 */
extern void vector_destroy(vector * _self);

/**
 * append an element at the end of the vector
 */
extern bool vector_append(vector * _self, void * _element);

/**
 * append elements from a vector at the end of this vector
 */
extern void vector_appendall(vector * _self, vector * _other);

/**
 * delete an element given by its reference (pointer).
 * a search for the element in the vector is performed.
 */
extern bool vector_delete(vector * _self, void * _element);

/**
 * get the element at a given location (index) in the vector.
 * Null is returned if bad index.
 */
extern void * vector_get(vector * _self, int _index);

/**
 * set the element at a given location (index) in the vector.
 * over writes existing element (if any).
 * inserts empty elements if location is beyond current size of vector.
 */
extern void vector_set(vector * _self, int _index, void * _element);

/**
 * get the number of elements in vector.
 * (including any null elements in the middle)
 */
extern int vector_getsize(vector * _self);

/**
 * is the vector empty of elements
 */
extern bool vector_isempty(vector * _self);

/**
 * get the index (location) of a given element given by its reference (pointer).
 * a search for the element in the vector is performed.
 * returns -1 if element not found.
 */
extern int vector_getindex(vector * _self, void * _element);

/**
 * Same as vector_getindex() with the addition of a specified element
 * comparison function
 */
extern int vector_getindexbycompare(vector * _self, void * _element,
		int (*element_compare)(void *, void *));

/**
 * remove the element at a given location (index) in the vector.
 * elements after removed element are sled down.
 */
extern bool vector_remove(vector * _self, int _index);

/**
 * insert the element at a given location (index) in the vector.
 * elements after removed element are moved up.
 * inserts empty elements if location is beyond current size of vector.
 */
extern bool vector_insert(vector * _self, int _index, void * _element);

/**
 * check if a given element given by its reference (pointer)
 * exists in the vector.
 * a search for the element in the vector is performed.
 */
extern bool vector_contains(vector * _self, void * _element);

/**
 * Same as vector_contains() with the addition of a specified element
 * comparison function
 */
extern bool vector_containsbycompare(vector * _self, void * _element,
		int (*element_compare)(void *, void *));

/**
 * clear all elements in the vector.
 * resize memory allocation to initial value
 */
extern void vector_clear(vector * _self);

/**
 * get an array representation of elements in the vector.
 * length of array is the current capacity of the vector.
 * number of elements in the array are known through vector_getsize().
 * (ARRAY  FOR READ ONLY, NO MODIFICATIONS)
 */
extern char ** vector_toarray(vector * _self);

/**
 * get an array copy of the elements in the vector.
 * length of array is the current size of the vector,
 * obtained through vector_getsize().
 * (freeing array is responsibility of caller)
 */
extern char ** vector_toarraycopy(vector * _self);

/**
 * toString()
 * provide function for printing an element object
 */
extern void vector_print(vector * _self, char* _name, void (*element_print)(void *));

/**
 * brief printing
 */
extern void vector_printbrief(vector * _self, void (*element_print)(void *));


#endif /* VECTOR_H_ */
