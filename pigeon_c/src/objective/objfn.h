/*
 * objfn.h
 *
 *  Created on: Nov 13, 2014
 *      Author: assertantawi
 */

#ifndef OBJFN_H_
#define OBJFN_H_

#include "commonobj.h"
#include "../system/defs.h"


struct s_objfn;

/**
 * The objective function to be optimized
 */
typedef struct s_objfn objfn;

/**
 * create an objective function
 */
extern objfn * objfn_create(cloudsystem * _cs, le * _le, objective_type _type, int _primeresourceindex);

/**
 * destroy the objective function
 */
extern void objfn_destroy(objfn * _self);

/**
 * get the (total) value of the objective function
 */
extern float objfn_getvalue(objfn * _self);

/**
 * get the value of the system component of the objective function
 */
extern float objfn_getsystemvalue(objfn * _self);

/**
 * get the base objective function
 */
extern void * objfn_getbasefunc(objfn * _self);


extern cloudsystem * objfn_getcloudsystem(objfn * _self);

extern int objfn_getresindex(objfn * _self);

/**
 * set print for debugging
 */
extern void objfn_setprint(objfn * _self, bool _isprint);

/**
 * print option for debugging
 */
extern bool objfn_isprint(objfn * _self);


#endif /* OBJFN_H_ */
