/*
 * hashset.h
 *
 *  Created on: May 31, 2018
 *      Author: tantawi
 */

#ifndef UTILS_HASHSET_H_
#define UTILS_HASHSET_H_

#include <stdbool.h>

#include "hashmap.h"
#include "vector.h"

struct s_hashset;

/**
 * A set of char* members backed by a hash map
 */
typedef struct s_hashset hashset;

/** Create a new hashset near the given size */
extern hashset* hashset_create(int _startsize);

/** Destroy the hashset structure */
extern void hashset_destroy(hashset* _self);

/** Add a new element into the hashset */
extern void hashset_add(hashset* _self, char* _elem);

/** Delete an element from the hashset; return true if found */
extern bool hashset_delete(hashset* _self, char* _elem);

/** Check if an element is in the hashset */
extern bool hashset_find(hashset* _self, char* _elem);

/** Return the number of elements in the hashset */
extern int hashset_count(hashset* _self);

/** Return a vector of all elements in the hashset
 * (freeing responsibility of caller)*/
extern vector* hashset_getelements(hashset* _self);

/** Print all elements in the hashset */
extern void hashset_print(hashset* _self);


#endif /* UTILS_HASHSET_H_ */
