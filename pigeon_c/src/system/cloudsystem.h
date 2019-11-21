/*
 * cloudsystem.h
 *
 *  Created on: Feb 10, 2014
 *      Author: assertantawi
 */

#ifndef CLOUDSYSTEM_H_
#define CLOUDSYSTEM_H_

#include "le.h"

#include "../utils/vector.h"
#include "../stats/demandstats.h"


struct s_cloudsystem;

/**
 * A model of the cloud system
 */
typedef struct s_cloudsystem cloudsystem;

/**
 * create a cloud system model from initial values
 * (Sequence of PEs in vector is assumed to be same as sequence of leaf nodes in tree model.)
 */
extern cloudsystem * cloudsystem_create(vector* _pes);

/**
 * destroy the cloud system model
 * (and all referenced objects)
 */
extern void cloudsystem_destroy(cloudsystem * _self);

/**
 * get number of PEs in cloud system model
 */
extern int cloudsystem_getnumPEs(cloudsystem * _self);

/**
 * get the PEs in the cloud system model
 */
extern vector * cloudsystem_getPEs(cloudsystem * _self);

/**
 * get the PE in the cloud system model with a particular id
 * (NULL if not found)
 */
extern void * cloudsystem_getPE(cloudsystem * _self, char* _peid);

/**
* return a PE by index
* (NULL if out of range)
*/
extern void * cloudsystem_getPE_byindex(cloudsystem* _self, int _index);

/**
* return PE name by index
* (NULL if out of range)
*/
extern char* cloudsystem_getPEname_byindex(cloudsystem* _self, int _index);

/**
 * add a PE to the existing cloud system model
 */
extern void cloudsystem_addPE(cloudsystem* _self, void* _pe);

/**
 * remove a PE from the existing cloud system model
 * all LEs on the PE will be unplaced
 */
extern void * cloudsystem_remove_PE(cloudsystem* _self, char* _peid);

/**
 * update the capacity and/or usage of an existing PE
 */
extern void cloudsystem_updatePE(cloudsystem* _self, char* _peid, int* _capacity, int* _usage);

/**
 * reset indices and level tree due to modifications to the PEs in the cloud system model
 */
extern void cloudsystem_resetpes(cloudsystem* _self);

/**
 * get the number of resources in a PE
 * assumes all PEs have same number or resources
 * (0 if no PEs)
 */
extern int cloudsystem_getnumresources(cloudsystem * _self);

/**
 * get average utilization of resources across PEs
 * (freeing responsibility of caller)
 */
extern float* cloudsystem_getutilization(cloudsystem * _self);

/**
 * get standard deviation of utilization of resources across PEs
 * (freeing responsibility of caller)
 */
extern float* cloudsystem_getutilizationstdev(cloudsystem * _self);

/**
 * the average capacity of resources across PEs
 * length is number of resources; entries may be zeroes
 * NULL if no PEs/resources
 * (freeing responsibility of caller)
 */
extern float* cloudsystem_getaveragecapacity(cloudsystem * _self);

extern void cloudsystem_printutilizationmatrix(cloudsystem * _self);

/**
 * display PE usage horizontal bars
 */
extern void cloudsystem_displayutilization(cloudsystem * _self, bool _issorted);

/**
 * display PE occupancy and resource usage
 */
extern void cloudsystem_displaysummary(cloudsystem * _self, bool _issorted);

/**
 * get the demand data statistics collector
 */
extern demandstats * cloudsystem_getdemandstats(cloudsystem * _self);

/**
 * set the demand data statistics collector
 * (held by the cloud system for convenience)
 */
extern void cloudsystem_setdemandstats(cloudsystem * _self, demandstats * _demandstats);


/**
 * toString()
 */
extern void cloudsystem_print(cloudsystem * _self);


#endif /* CLOUDSYSTEM_H_ */
