/*
 * abpfn.h
 *
 *  Created on: Apr 20, 2017
 *  Author: assertantawi
 */

#ifndef OBJECTIVE_ABPFN_H_
#define OBJECTIVE_ABPFN_H_

#include "objfn.h"

/**
 * whether to use historical or current resource demand
 */
#define ABPN_USE_STATISTICS 1

/**
 * whether to use a scalar value or matrix for variation comparison
 */
#define ABPN_COMPARE_SCALAR 1


struct s_abpfn;

/**
 * An Adaptive Bin Packing (ABP) objective function
 */
typedef struct s_abpfn abpfn;

/**
 * create an ABP objective function
 */
extern abpfn * abpfn_create();

/**
 * destroy the objective function
 */
extern void abpfn_destroy(void * _self);

/**
 * get the value of the system component of the objective function
 */
extern float abpfn_getsystemvalue(objfn * _self);


#endif /* OBJECTIVE_ABPFN_H_ */
