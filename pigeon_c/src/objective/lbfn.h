/*
 * lbfn.h
 *
 *  Created on: Nov 14, 2014
 *      Author: assertantawi
 */

#ifndef LBFN_H_
#define LBFN_H_

#include "objfn.h"

struct s_lbfn;

/**
 * A load balancing objective function
 */
typedef struct s_lbfn lbfn;

/**
 * create a load balancing objective function
 */
extern lbfn * lbfn_create();

/**
 * destroy the load balancing function
 */
extern void lbfn_destroy(void * _self);

/**
 * get the value of the system component of the load balancing objective function
 */
extern float lbfn_getsystemvalue(objfn * _self);


#endif /* LBFN_H_ */
