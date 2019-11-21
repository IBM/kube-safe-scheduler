/*
 * integer.h
 *
 *  Created on: Jan 30, 2018
 *      Author: tantawi
 */

#ifndef UTILS_INTEGER_H_
#define UTILS_INTEGER_H_

struct s_integer;

/**
 * An Integer object
 */
typedef struct s_integer integer;

/**
 * create a integer
 */
extern integer* integer_create(int _value);

/**
 * destroy the integer
 */
extern void integer_destroy(integer* _self);

/**
 * get the value
 */
extern int integer_value(integer* _self);

/*
 * change the value
 */
extern void integer_set(integer* _self, int _value);

/**
 * compare function
 */
extern int integer_compare(void* _self, void* _other);

/**
 * toString()
 */
extern void integer_print(integer* _self);

#endif /* UTILS_INTEGER_H_ */
