/*
 * hashmap.h
 *
 *  Created on: Feb 15, 2014
 *      Author: assertantawi
 */

/**
 * http://www2.informatik.hu-berlin.de/~weber/slipOff/hashmap_h.html
 *
 * modified
 */

#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "vector.h"

struct s_hashmap;

/** Hashmap structure (forward declaration) */
typedef struct s_hashmap hashmap;

/** Create a new hashmap near the given size. */
extern hashmap* hashmap_create(int _startsize);

/** Destroy the hashmap structure. */
extern void hashmap_destroy(hashmap* _self);

/** Insert a new element into the hashmap. */
extern void hashmap_insert(hashmap* _self, const void* _data, char* _keystring);

/** Remove the storage for the element of the key and return the element. */
extern void* hashmap_remove(hashmap* _self, char* _keystring);

/** Return the element for the key. */
extern void* hashmap_get(hashmap* _self, char* _keystring);

/** Return the number of saved elements. */
extern long hashmap_count(hashmap* _self);

/** Return a vector of all values in the hashmap.
 * (freeing responsibility of caller)*/
extern vector* hashmap_getvalues(hashmap* _self);

/** Return a vector of all keys in the hashmap.
 * (freeing responsibility of caller)*/
extern vector* hashmap_getkeys(hashmap* _self);

/** Print all content values in the hashmap using provided data_print
 * function for printing a data element. */
extern void hashmap_print(hashmap* _self, void (*data_print)(void *));

/** Brief printing */
extern void hashmap_printbrief(hashmap* _self, void (*data_print)(void *));

/** Hash function from string to long */
extern unsigned long hashfunction(char* _datum);

#endif /* HASHMAP_H_ */
