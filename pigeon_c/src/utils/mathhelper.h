/*
 * mathhelper.h
 *
 *  Created on: Feb 11, 2014
 *      Author: assertantawi
 */

#ifndef MATHHELPER_H_
#define MATHHELPER_H_

#include <stdbool.h>

/**
 * Math helper functions
 * (Floating point types are assumed, unless specified)
 */

/**
 * Is the newValue and oldValue close enough within the given relativeErrorTolerance
 */
extern bool mathhelper_arePracticallyEqual(float _newValue, float _oldValue,
		float relativeErrorTolerance);

/**
 * normalize an array of values to sum up to one,
 * if all values are zero then do nothing
 * (return false if all values are zero)
 */
extern bool mathhelper_normalizearray(float* _x, int _n);

/**
 * add array x to array y, both of length n (element-wise)
 * y = y + x
 */
extern void mathhelper_addarray(float* _y, float* _x, int _n);

/**
 * multiply array x to array y, both of length n (element-wise)
 * y = y * x
 */
extern void mathhelper_multiplyarray(float* _y, float* _x, int _n);

/**
 * sum all elements in an array
 */
extern double mathhelper_sumarray(float* _x, int _n);

/**
 * set all elements in an array to a given value
 */
extern void mathhelper_setarray(float* _x, int _n, float _v);

/**
 * multiply all elements in an array by a given value
 */
extern void mathhelper_scalearray(float* _x, int _n, float _v);

/**
 * shift all elements in an array by a given value
 */
extern void mathhelper_shiftarray(float* _x, int _n, float _v);

/**
 * copy (clone) array
 * (freeing output responsibility of caller)
 */
extern float* mathhelper_copyarray(float* _x, int _n);

/**
 * add a scalar value to all elements in an array
 */
extern void mathhelper_array_add_scalar(double* _x, int _n, double _v);

/**
 * calculate the average of an array of numbers
 */
extern double mathhelper_average(float* _x, int _n);

/**
 * calculate standard deviation of an array of numbers
 */
extern double mathhelper_stddev(float* _x, int _n);

/**
 * compare two floats
 */
extern int mathhelper_floatcomp(const void* _elem1, const void* _elem2);


#endif /* MATHHELPER_H_ */
