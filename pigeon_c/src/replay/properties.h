/*
 * properties.h
 *
 *  Created on: Aug 22, 2014
 *      Author: assertantawi
 */

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <stdbool.h>

#include "../utils/vector.h"

#define PROP_MAX_LINE_LENGTH	4096
#define PROP_MAX_KEY_LENGTH		64
#define PROP_MAX_VALUE_LENGTH	4096

#define PROP_COMMENT_LINE	'#'

struct s_properties;

/**
 * properties list
 */
typedef struct s_properties properties;

/**
 * create a properties list
 */
extern properties * properties_create();

/**
 * destroy the properties list
 */
extern void properties_destroy(properties * _self);

/**
 * read properties from file
 * return false if read fails
 */
extern bool properties_readfromfile(properties * _self, char * _filename);

/**
 * get the value of property with name key
 * return false if not found
 */
extern bool properties_get(properties * _self, char * _key, char * _value);

/**
 * get a vector of all keys
 */
extern vector *  properties_getkeys(properties * _self);

/**
 * clear all properties
 */
extern void properties_clear(properties * _self);

/**
 * toString()
 * print all properties and their values as key value pairs
 */
extern void properties_print(properties * _self);


#endif /* PROPERTIES_H_ */
