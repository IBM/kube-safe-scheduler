/*
 * resource.h
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdbool.h>

#include "le.h"

struct s_resource;

/**
 * Resources of different types held by a PE
 */
typedef struct s_resource resource;

/**
 * create a resource from initial parameter values.
 * The parameters are:
 * - number of resource types;
 * - capacities of the resource types;
 * - whether resource overflow is allowed for the resource types
 * (if NULL then overflow is not allowed for all resource types).
 *
 * (parameter arrays are copied)
 */
extern resource * resource_create(int _num, int* _capacity, bool* _allowoverflow);

/**
 * destroy the resource
 */
extern void resource_destroy(resource * _self);

/**
 * check if a given LE could use this resource,
 * but do not use it
 */
extern bool resource_canuse(resource * _self, le * _le);

/**
 * Let a given LE use this resource
 * return true if success and false otherwise
 */
extern bool resource_use(resource * _self, le * _le);

/**
 * remove a LE from using this resource
 * return true if removed and false if a resource under-load occurred
 */
extern bool resource_release(resource * _self, le * _le);

/**
 * get number of resource types
 */
extern int resource_getnum(resource * _self);

/**
 * array of resource utilization (fractional values)
 * (freeing responsibility of caller)
 */
extern float* resource_getutilization(resource * _self);

/**
 * array of resource over-utilization (fractional values)
 * (freeing responsibility of caller)
 */
extern float* resource_getoverutilization(resource * _self);

/**
 * array of resource capacity (absolute values)
 */
extern int* resource_getcapacity(resource * _self);

/**
 * set resource capacity (absolute values)
 * (capacity array is copied)
 */
extern void resource_setcapacity(resource * _self, int* _capacity);

/**
 * array of resource usage (absolute values)
 */
extern int* resource_getusage(resource * _self);

/**
 * set resource usage (absolute values)
 * (usage array is copied)
 */
extern void resource_setusage(resource * _self, int* _usage);

/**
 * increment resource usage by a delta usage
 */
extern void resource_incrementusage(resource * _self, int* _deltausage);

/**
 * array of resource availability (absolute values)
 * (freeing responsibility of caller)
 */
extern int* resource_getavailability(resource * _self);

/**
 * check if usage is within capacity
 */
extern bool resource_isnormalload(resource * _self);

/**
 * whether overflow is allowed
 */
extern bool* resource_allowoverflow(resource * _self);

/**
 * toString()
 */
extern void resource_print(resource * _self);

#endif /* RESOURCE_H_ */
