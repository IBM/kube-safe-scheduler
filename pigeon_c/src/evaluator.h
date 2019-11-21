/*
 * evaluator.h
 *
 *  Created on: Feb 27, 2014
 *      Author: assertantawi
 */

#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <stdbool.h>

#include "system/defs.h"

struct s_evaluator;

/**
 * A system evaluator for scheduler
 */
typedef struct s_evaluator evaluator;


/**
 * create an evaluator from parameters
 */
extern evaluator * evaluator_create(int _policyresourceindex, objective_type _policyobjective);

/**
 * create an evaluator using parameters from a file
 */
extern evaluator * evaluator_createfromfile(char* _fname);

/**
 * destroy the evaluator
 */
extern void evaluator_destroy(evaluator * _self);

/**
 * calculate objective function for placement of an LE on all PEs in the system
 * (NULL if null system or LE; NAN entry if cannot place)
 * (Freeing responsibility of caller)
 */
extern float* evaluator_eval(evaluator * _self, void* _le);

/**
 * get the model state updater
 */
extern void * evaluator_getstateupdater(evaluator * _self);

/**
 * get the number of resource types
 */
extern int evaluator_getnumresources(evaluator * _self);

/**
 * get the policy objective
 */
extern char* evaluator_getpolicyobjective(evaluator * _self);

/**
 * set the policy objective
 */
extern void evaluator_setpolicyobjective(evaluator * _self, char* _policyobjective);


/**
 * toString()
 */
extern void evaluator_print(evaluator * _self);


#endif /* EVALUATOR_H_ */
