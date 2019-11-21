/*
 * consfn.h
 *
 *  Created on: Nov 20, 2014
 *      Author: assertantawi
 */

#ifndef CONSFN_H_
#define CONSFN_H_

#include "objfn.h"

struct s_consfn;

/**
 * A consolidation objective function
 */
typedef struct s_consfn consfn;

/**
 * create a consolidation  objective function
 */
extern consfn * consfn_create();

/**
 * destroy the consolidation objective function
 */
extern void consfn_destroy(void * _self);

/**
 * get the value of the system component of the consolidation objective function
 */
extern float consfn_getsystemvalue(objfn * _self);

#endif /* CONSFN_H_ */
