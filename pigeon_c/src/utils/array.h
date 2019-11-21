/*
 * array.h
 *
 *  Created on: Nov 2, 2018
 *      Author: tantawi
 */

#ifndef UTILS_ARRAY_H_
#define UTILS_ARRAY_H_

#include <stdbool.h>


struct s_array;

/**
 * An array of numbers
 */
typedef struct s_array array;

/**
 * create an array of a given length, set to zero
 */
extern array * array_create(int _n);

/**
 * create an array given data
 */
extern array * array_createfromdata(int _n, float * _data);

/**
 * destroy the array
 */
extern void array_destroy(array * _self);

/**
 * the length of the array
 */
extern int array_getlength(array * _self);

/**
 * the data in the array
 */
extern float* array_getdata(array * _self);

/**
 * get the i_th data element in the array, 0 if out of range
 */
extern float array_getelement(array * _self, int _i);

/**
 * set the i_th data element in the array
 */
extern void array_setelement(array * _self, int _i, float _value);

/**
 * set all elements in the array to a given value
 */
extern void array_set(array * _self, float _value);

/**
 * clone into new array
 * (freeing output responsibility of caller)
 */
extern array * array_clone(array * _self);

/**
 * sum all elements in the array
 */
extern double array_sum(array * _self);

/**
 * sum first n elements in the array
 */
extern double array_sumN(array * _self, int _n);

/**
 * multiply all elements in the array by a given value
 */
extern void array_scale(array * _self, float _value);

/**
 * shift all elements in the array by a given value
 */
extern void array_shift(array * _self, float _value);

/**
 * get index of minimum element
 * (-1 if empty)
 */
extern int array_minindex(array * _self);

/**
 * get index of maximum element
 * (-1 if empty)
 */
extern int array_maxindex(array * _self);

/**
 * is this array equal to other array (length and element wise)
 */
extern bool array_isequal(array * _self, array * _other);

/**
 * copy other array into this array
 */
extern void array_copy(array * _self, array * _other);

/**
 * add other array to this array (element-wise)
 * assuming both arrays have same length
 */
extern void array_add(array * _self, array * _other);

/**
 * multiply other array with this array (element-wise)
 * assuming both arrays have same length
 */
extern void array_multiply(array * _self, array * _other);

/**
 * normalize array to sum up to one,
 * if all values are zero then do nothing
 * (return false if all values are zero)
 */
extern bool array_normalize(array * _self);

/**
 * calculate the average of the numbers in the array
 */
extern double array_average(array * _self);

/**
 * calculate the standard deviation of the numbers in the array
 */
extern double array_stddev(array * _self);


/**
 * toString()
 */
extern void array_print(array * _self);

#endif /* UTILS_ARRAY_H_ */
