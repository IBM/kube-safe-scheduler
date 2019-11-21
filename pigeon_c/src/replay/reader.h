/*
 * reader.h
 *
 *  Created on: Sep 4, 2014
 *      Author: assertantawi
 */

#ifndef READER_H_
#define READER_H_

#include <stdbool.h>

#include "../evaluator.h"

struct s_reader;

/**
 * A configuration reader
 *
 * An example follows:
 *
 *<pre>
 ######################################################################
 # parameters
 ######################################################################
 policyResourceIndex = 0
 policyObjective = LOAD_BALANCE
 </pre>
 */
typedef struct s_reader reader;

/**
 * create a reader
 */
extern reader * reader_create(char* _fname);

/**
 * destroy the reader
 */
extern void reader_destroy(reader * _self);

/**
 * get the policy resource index
 */
extern int reader_getpolicyresourceindex(reader * _self);

/**
 * get the policy objective
 */
extern objective_type reader_getpolicyobjective(reader * _self);

/**
 * get a particular property value from the input file given the key
 */
extern bool reader_getproperty(reader * _self, char * _key, char * _value);

/**
 * toString()
 */
extern void reader_print(reader * _self);

#endif /* READER_H_ */
