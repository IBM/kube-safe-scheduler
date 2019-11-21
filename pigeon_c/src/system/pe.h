/*
 * pe.h
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#ifndef PE_H_
#define PE_H_

#include <stdbool.h>

#include "resource.h"
#include "../utils/vector.h"

struct s_pe;

/**
 * Physical Entity (PE)
 */
typedef struct s_pe pe;


/**
 * create a PE from initial values
 */
extern pe * pe_create(char* _name, resource * _resource);

/**
 * destroy the PE
 */
extern void pe_destroy(pe* _self);

/**
 * name id of the PE
 */
extern char * pe_getname(pe * _self);

/**
 * comparison function of two PEs, based on name
 */
extern int pe_comparePEs(const void * _pe1, const void * _pe2);

/**
 * check if a given LE could be placed on this PE
 * (but do not place it)
 * return true if can be placed and false otherwise
 */
extern bool pe_canplaceLE(pe* _self, le* _le);

/**
 * place a given LE onto this PE
 * (assume that it is place-able)
 * optional update of resource usage
 * return true if placed and false if a resource overload occurred
 */
extern bool pe_placeLE(pe* _self, le* _le, bool _update_resource);

/**
 * remove a LE from this PE
 * optional update of resource usage
 * return true if removed and false if a resource under-load occurred
 */
extern bool pe_removeLE(pe* _self, le* _le, bool _update_resource);

/**
 * get a list of hosted LEs
 */
extern vector * pe_gethostedLE(pe* _self);


/**
 * clear PE from all hosted LEs
 */
extern void pe_clear(pe* _self, bool _update_resource);

/**
 * get the resources of this PE
 */
extern resource * pe_getresource(pe* _self);

/**
 * update the capacity and/or usage
 */
extern void pe_update(pe* _self, int* _capacity, int* _usage);

/**
 * get the utilization of the prime resource, given by its index, on this PE
 */
extern float pe_getprimeutilization(pe* _self, int _res_index);

/**
 * set the index of PE in the cloud system
 */
extern int pe_getindexinsystem(pe * _self);

/**
 * get the index of PE in the cloud system
 */
extern void pe_setindexinsystem(pe * _self, int _indexinsystem);

/**
 * toString()
 */
extern void pe_print(pe * _self);


#endif /* PE_H_ */
