/*
 * commonobj.h
 *
 *  Created on: Nov 14, 2014
 *      Author: assertantawi
 */

#ifndef COMMONOBJ_H_
#define COMMONOBJ_H_

#include "../system/cloudsystem.h"
#include "../system/le.h"


/**
 * compute value function given array x of length n,
 * with values in [0,1]
 * 		objective: balance
 */
extern double commonobj_balance(float* x, int n);


/** types of un-balancing criteria */
typedef enum {
	UNBALANCE_BASIC,
	UNBALANCE_STDEV,
	UNBALANCE_SCALED,
	UNBALANCE_MIXED
} commonobj_unbalance_type;

/**
 * compute value function given array x of length n,
 * with values in [0,1]
 *		objective: un-balance, i.e. consolidate
 */
extern double commonobj_unbalance(float* x, int n, commonobj_unbalance_type type);


#endif /* COMMONOBJ_H_ */
