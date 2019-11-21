/*
 * matrix.h
 *
 *  Created on: Nov 2, 2018
 *      Author: tantawi
 */

#ifndef UTILS_MATRIX_H_
#define UTILS_MATRIX_H_

#include <stdbool.h>

#include "array.h"


struct s_matrix;

/**
 * methods for calculating a scalar
 * coefficient of variation for multivariates
 *
 * - Method of Albert and Zhang, Biom J. 2010 Oct;52(5):667-75
 * "A novel definition of the multivariate coefficient of variation"
 *
 * - Other methods described in
 * "Multivariate coefficients of variation: comparison and influence functions"
 * S Aerts, G Haesbroeck, C Ruwet - Journal of Multivariate Analysis, 2015 - Elsevier
 **/
typedef enum {
	ALBERT_ZHANG,
	VOINOV_NIKULIN
} matrix_cov_method;

/**
 * An matrix of numbers
 */
typedef struct s_matrix matrix;

/**
 * create a matrix of a given size, set to zero
 */
extern matrix * matrix_create(int _nrows, int _ncols);

/**
 * create a matrix from data
 */
extern matrix * matrix_createfromdata(int _nrows, int _ncols, float** _data);

/**
 * create a square matrix of a given size, set to zero
 */
extern matrix * matrix_create_square(int _n);

/**
 * create an identity matrix of a given size
 */
extern matrix * matrix_create_identity(int _n);

/**
 * destroy the matrix
 */
extern void matrix_destroy(matrix * _self);

/**
 * the number of rows in the matrix
 */
extern int matrix_getnumrows(matrix * _self);

/**
 * the number of columns in the matrix
 */
extern int matrix_getnumcols(matrix * _self);

/**
 * is the matrix a square one
 */
extern bool matrix_issquare(matrix * _self);

/**
 * the data in the matrix
 */
extern float** matrix_getdata(matrix * _self);

/**
 * get data element [i][j] in the matrix, 0 if out of range
 */
extern float matrix_getelement(matrix * _self, int _i, int _j);

/**
 * set data element [i][j] in the matrix
 */
extern void matrix_setelement(matrix * _self, int _i, int _j, float _value);

/**
 * clone into new matrix
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_clone(matrix * _self);

/**
 * copy other matrix into this matrix
 */
extern void matrix_copy(matrix * _self, matrix * _other);

/**
 * get the trace of the matrix
 */
extern double matrix_trace(matrix * _self);

/**
 * transpose a matrix x [m x n]
 * output: z [n x m]
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_transpose(matrix * _self);

/**
 * scale a matrix by a value (element wise)
 */
extern void matrix_scale(matrix * _self, float _value);

/*
 * add other matrix to this matrix (assuming same size) element-wise
 */
extern void matrix_add(matrix * _self, matrix * _other);

/*
 * multiply two matrices x [m x k] by y [k x n]
 * output: z [m x n]
 * return NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_multiply(matrix * _self, matrix * _other);

/*
 * multiply this matrix by a vector from the left
 * input: y [k x n] and x [k]
 * output: z[n] = x[k] * y[k,n]
 * return NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
extern array * matrix_multiply_left(matrix * _self, array * _other);

/*
 * multiply this matrix by a vector from the left
 * input: x [m x k] and y [k]
 * output: z[m] = x[m,k] * y[k]
 * return NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
extern array * matrix_multiply_right(matrix * _self, array * _other);

/**
 * calculate the multivariate average of data points
 * where p is the multivariate dimension
 *       n is the number of data points
 * input: x [p x n]
 * output: y [p]
 * (freeing output responsibility of caller)
 */
extern array * matrix_average(matrix * _self);

/**
 * calculate the multivariate covariance matrix of data points
 * where p is the multivariate dimension
 *       n is the number of data points
 * input: x [p x n]
 * output: y [p x p]
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_covariance(matrix * _self);

/**
 * same as matrix_covariance() but using already
 * computed second moment mu2 matrix and average mu array
 * return NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_covariance_from_stats(matrix * _mu2, array * _mu);

/**
 * calculate the coefficient of variation of a multivariate set of data points
 * where p is the multivariate dimension
 *       n is the number of data points
 * input: x [p x n]
 */
extern double matrix_coefficient_variation(matrix * _self,
		matrix_cov_method _method);

/**
 * same as matrix_coefficient_variation() but using already
 * computed covariance sigma matrix and average mu array
 */
extern double matrix_coefficient_variation_from_stats(matrix * _sigma, array * _mu,
		matrix_cov_method _method);

/**
 * calculate the coefficient of variation (squared) matrix given
 * the covariance sigma matrix and average mu array, where
 * elements of the matrix are the joint covariance over the squared mean
 * (note some entries could be negative, thus no attempt to take square root)
 * return NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
extern matrix* matrix_cov2_matrix(matrix * _sigma, array * _mu);


/**
 * get the determinant of a matrix
 * (0 if not matrix not square)
 */
extern float matrix_determinant(matrix * _self);

/**
 * get the inverse matrix of a matrix
 * return NULL if singular or bad dimensions
 * (freeing output responsibility of caller)
 */
extern matrix * matrix_inverse(matrix * _self);


/**
 * toString()
 */
extern void matrix_print(matrix * _self);

#endif /* UTILS_MATRIX_H_ */
