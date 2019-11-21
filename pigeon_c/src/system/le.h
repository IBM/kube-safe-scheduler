/*
 * ve.h
 *
 *  Created on: Feb 6, 2014
 *      Author: assertantawi
 */

#ifndef LE_H_
#define LE_H_

#include <stdbool.h>

#include "../utils/maparray.h"
#include "../utils/vector.h"

struct s_le;

/**
 *  Logical Entity (LE)
 *  (extends graph node)
 */
typedef struct s_le le;

/**
 * create a LE from initial values
 * (demand is copied)
 */
extern le * le_create(char* _name, int * _demand, int _numdemand);

/**
 * destroy the LE
 */
extern void le_destroy(le * _self);

/**
 * name id of the LE
 */
extern char * le_getname(le * _self);

/**
 * is the LE assigned a host
 */
extern bool le_ishosted(le * _self);

/**
 * get the host of this LE
 */
extern void * le_gethost(le * _self);

/**
 * set the host for this LE
 */
extern void le_sethost(le * _self, void * _pe);

/**
 * get the resource demands of this LE
 */
extern int* le_getdemand(le * _self);

/**
 * get the number of resources in the resource demand array
 */
extern int le_getnumdemand(le * _self);

/**
 * toString()
 */
extern void le_print(le * _self);

#endif /* LE_H_ */
