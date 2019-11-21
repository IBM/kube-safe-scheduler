/*
 * matrix.c
 *
 *  Created on: Nov 2, 2018
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "matrix.h"

/**
 * An matrix of numbers
 */
struct s_matrix {
	/** number of rows */
	int nrows;
	/** number of columns */
	int ncols;
	/** array of elements */
	float **x;
	/** allocated space */
	bool allocated;
};

matrix * matrix_create(int _nrows, int _ncols) {
	float** z = (float**) malloc(_nrows * sizeof(float*));
	assert(z);
	for (int i = 0; i < _nrows; ++i) {
		z[i] = (float*) calloc(_ncols, sizeof(float));
		assert(z[i]);
	}

	matrix* m = matrix_createfromdata(_nrows, _ncols, z);
	m->allocated = true;
	return m;
}

matrix * matrix_createfromdata(int _nrows, int _ncols, float** _data) {
	matrix * mat = (void *) malloc(sizeof(matrix));
	assert(mat);
	mat-> allocated = false;

	if (_nrows <= 0 || _ncols <= 0) {
		mat->nrows = 0;
		mat->ncols = 0;
		mat->x = NULL;
		return mat;
	}

	mat->nrows = _nrows;
	mat->ncols = _ncols;
	mat->x = _data;

	return  mat;
}

matrix * matrix_create_square(int _n){
	return matrix_create(_n, _n);
}

matrix * matrix_create_identity(int _n) {
	matrix* mat = matrix_create_square(_n);
	for (int i = 0; i < _n; ++i) {
		mat->x[i][i] = 1;
	}
	return mat;
}

void matrix_destroy(matrix * _self) {
	if (_self) {
		if (_self->allocated &&_self->x) {
			for (int i = 0; i < _self->nrows; ++i) {
				free(_self->x[i]);
			}
			free(_self->x);
			_self->x = NULL;
		}

		free(_self);
	}
}

int matrix_getnumrows(matrix * _self) {
	return _self->nrows;
}

int matrix_getnumcols(matrix * _self) {
	return _self->ncols;
}
bool matrix_issquare(matrix * _self) {
	return _self->nrows == _self->ncols;
}

float** matrix_getdata(matrix * _self) {
	return _self->x;
}

float matrix_getelement(matrix * _self, int _i, int _j) {
	if (_i < 0 || _i >= _self->nrows || _j < 0 || _j >= _self->ncols) {
		return 0;
	}
	return _self->x[_i][_j];
}

void matrix_setelement(matrix * _self, int _i, int _j, float _value) {
	if (_i >= 0 && _i < _self->nrows && _j >= 0 && _j < _self->ncols) {
		_self->x[_i][_j] = _value;
	}
}

matrix * matrix_clone(matrix * _self) {
	matrix* other = matrix_create(_self->nrows, _self->ncols);
	matrix_copy(other, _self);
	return other;
}

void matrix_copy(matrix * _self, matrix * _other) {
	if ((matrix_getnumrows(_other) != matrix_getnumrows(_self))
			|| (matrix_getnumcols(_other) != matrix_getnumcols(_self))) {
		return;
	}
	float** y = matrix_getdata(_other);
	for (int i = 0; i < _self->nrows; i++) {
		for (int j = 0; j < _self->ncols; ++j) {
			_self->x[i][j] = y[i][j];
		}
	}
}

double matrix_trace(matrix * _self) {
	double trace = 0;
	if (matrix_issquare(_self)) {
		for (int i = 0; i < _self->nrows; ++i) {
			trace += _self->x[i][i];
		}
	}
	return trace;
}

matrix * matrix_transpose(matrix * _self) {
	int m = _self->nrows;
	int n = _self->ncols;

	matrix* mat = matrix_create(n, m);
	float** z = matrix_getdata(mat);
	float** x = matrix_getdata(_self);

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			z[i][j] = x[j][i];
		}
	}
	return mat;
}

void matrix_scale(matrix * _self, float _value) {
	int m = _self->nrows;
	int n = _self->ncols;
	float** x = matrix_getdata(_self);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			x[i][j] *= _value;
		}
	}
}

void matrix_add(matrix * _self, matrix * _other) {
	int m = _self->nrows;
	int n = _self->ncols;
	if (matrix_getnumrows(_other) != m || matrix_getnumcols(_other) != n) {
		return;
	}

	float** x = matrix_getdata(_self);
	float** y = matrix_getdata(_other);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			x[i][j] += y[i][j];
		}
	}
}

matrix * matrix_multiply(matrix * _self, matrix * _other) {
	int m = _self->nrows;
	int k = _self->ncols;
	int n = matrix_getnumcols(_other);

	if (matrix_getnumrows(_other) != k) {
		return NULL;
	}

	matrix* mat = matrix_create(m, n);
	float** z = matrix_getdata(mat);
	float** x = matrix_getdata(_self);
	float** y = matrix_getdata(_other);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			for (int l = 0; l < k; ++l) {
				z[i][j] += x[i][l] * y[l][j];
			}
		}
	}
	return mat;
}

array * matrix_multiply_left(matrix * _self, array * _other) {
	int k = _self->nrows;
	int n = _self->ncols;

	if (array_getlength(_other) != k) {
		return NULL;
	}

	array* a = array_create(n);
	float* z = array_getdata(a);
	float* x = array_getdata(_other);
	float** y = matrix_getdata(_self);

	for (int i = 0; i < n; ++i) {
		for (int l = 0; l < k; ++l) {
			z[i] += x[l] * y[l][i];
		}
	}
	return a;
}

array * matrix_multiply_right(matrix * _self, array * _other) {
	int m = _self->nrows;
	int k = _self->ncols;

	if (array_getlength(_other) != k) {
		return NULL;
	}

	array* a = array_create(m);
	float* z = array_getdata(a);
	float** x = matrix_getdata(_self);
	float* y = array_getdata(_other);

	for (int i = 0; i < m; ++i) {
		for (int l = 0; l < k; ++l) {
			z[i] += x[i][l] * y[l];
		}
	}
	return a;
}

array * matrix_average(matrix * _self) {
	int p = _self->nrows;
	int n = _self->ncols;

	array* a = array_create(p);

	if (n == 0) {
		return a;
	}

	float* z = array_getdata(a);
	float** x = matrix_getdata(_self);

	for (int i = 0; i < p; ++i) {
		double sum = 0;
		for (int j = 0; j < n; ++j) {
			sum += x[i][j];
		}
		z[i] = sum / n;
	}
	return a;
}

matrix * matrix_covariance(matrix * _self) {
	int p = _self->nrows;
	int n = _self->ncols;

	matrix* cov = matrix_create_square(p);
	float** z = matrix_getdata(cov);

	if (n == 0) {
		return cov;
	}

	array* avg = matrix_average(_self);
	float* mu = array_getdata(avg);
	float** x = matrix_getdata(_self);

	for (int i = 0; i < p; ++i) {
		for (int j = 0; j <= i; ++j) {
			double sum2 = 0;
			for (int l = 0; l < n; ++l) {
				sum2 += x[i][l] * x[j][l];
			}
			z[i][j] = ((sum2 / n) - (mu[i] * mu[j]));
			z[j][i] = z[i][j];
		}
	}

	array_destroy(avg);
	return cov;
}

matrix * matrix_covariance_from_stats(matrix * _mu2, array * _mu) {
	if (!matrix_issquare(_mu2)
			|| (matrix_getnumrows(_mu2) != array_getlength(_mu))
			|| (array_getlength(_mu) == 0)) {
		return NULL;
	}

	int p = matrix_getnumrows(_mu2);
	matrix* cov = matrix_create_square(p);
	float** c = matrix_getdata(cov);

	float* a = array_getdata(_mu);
	float** b = matrix_getdata(_mu2);

	for (int i = 0; i < p; ++i) {
		for (int j = 0; j <= i; ++j) {
			c[i][j] = b[i][j] - (a[i] * a[j]);
			c[j][i] = c[i][j];
		}
	}

	return cov;
}

double matrix_coefficient_variation(matrix * _self, matrix_cov_method _method) {
	if (_self->ncols == 0) {
		return 0;
	}

	array* avg = matrix_average(_self);
	matrix* cov = matrix_covariance(_self);

	double c = matrix_coefficient_variation_from_stats(cov, avg, _method);

	matrix_destroy(cov);
	array_destroy(avg);

	return c;
}

double matrix_coefficient_variation_from_stats(matrix * _sigma, array * _mu, matrix_cov_method _method) {

	int p = array_getlength(_mu);
	if (!matrix_issquare(_sigma)
			|| (matrix_getnumrows(_sigma) != p)
			|| (p == 0)) {
		return 0;
	}

	/*
	 * Assumption: sigma is a positive-definite matrix
	 */
	double c = 0;
	/**
	 * case: single variable
	 */
	if (p == 1) {
		float var = matrix_getelement(_sigma, 0, 0);
		float mu = array_getelement(_mu, 0);
		c = (mu != 0 && var >= 0) ? sqrt(var) / mu : 0;
		return c;
	}

	/**
	 * case: multiple variables
	 */
	switch (_method) {

	/**
	 * Albert and Zhang
	 */
	case ALBERT_ZHANG:
	default: {
		array* prod1 = matrix_multiply_left(_sigma, _mu);
		array_multiply(prod1, _mu);
		double nom = array_sum(prod1);
		nom = fmax(nom, 0);

		array* prod2 = array_clone(_mu);
		array_multiply(prod2, _mu);
		double denom = array_sum(prod2);

		c = (denom > 0 && nom > 0) ? sqrt(nom) / denom : 0;

		array_destroy(prod2);
		array_destroy(prod1);
	}
		break;

		/*
		 * Voinov and Nikulin
		 */
	case VOINOV_NIKULIN: {
		matrix* invSigma = matrix_inverse(_sigma);
		if (!invSigma) {
			return 0;
		}
		array* prod1 = matrix_multiply_left(invSigma, _mu);
		array_multiply(prod1, _mu);
		double denom = array_sum(prod1);

		c = (denom > 0) ? 1 / sqrt(denom) : 0;

		array_destroy(prod1);
		matrix_destroy(invSigma);
	}
		break;
	}

	return c;
}

matrix* matrix_cov2_matrix(matrix * _sigma, array * _mu) {
	int p = array_getlength(_mu);
	if (!matrix_issquare(_sigma)
			|| (matrix_getnumrows(_sigma) != p)
			|| (p == 0)) {
		return NULL;
	}

	matrix* cov = matrix_create_square(p);
	float** c = matrix_getdata(cov);

	float* a = array_getdata(_mu);
	float** b = matrix_getdata(_sigma);

	for (int i = 0; i < p; ++i) {
		for (int j = 0; j < p; ++j) {
			c[i][j] = b[i][j] / (a[i] * a[j]);
		}
	}

	return cov;
}

/**
 * get the cofactor matrix around a point (p, q) in a matrix
 * NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
matrix * matrix_cofactor(matrix * _self, int _p, int _q) {
	int n = matrix_getnumrows(_self);
	if (!matrix_issquare(_self) || n <= 1 || _p < 0 || _p >= n || _q < 0
			|| _q >= n) {
		return NULL;
	}

	matrix* c = matrix_create_square(n - 1);
	float** y = matrix_getdata(c);
	float** x = matrix_getdata(_self);

	int row = 0;
	int col = 0;
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (i != _p && j != _q) {
				y[row][col] = x[i][j];
				col++;
				if (col == n - 1) {
					col = 0;
					row++;
				}
			}
		}
	}

	return c;
}

float matrix_determinant(matrix * _self) {
	double d = 0;
	if (!matrix_issquare(_self)) {
		return d;
	}

	int n = matrix_getnumrows(_self);
	float** x = matrix_getdata(_self);
	if (n == 1) {
		return x[0][0];
	}

	int sign = 1;
	for (int j = 0; j < n; ++j) {
		matrix* cof = matrix_cofactor(_self, 0, j);
		if (cof) {
			d += sign * x[0][j] * matrix_determinant(cof);
			sign = -sign;
			matrix_destroy(cof);
		}
	}

	return (float) d;
}

/**
 * the adjoint matrix of a matrix
 * A . adj(A) = det(A) . I
 * NULL if bad dimensions
 * (freeing output responsibility of caller)
 */
matrix * matrix_adjoint(matrix * _self) {
	if (!matrix_issquare(_self)) {
		return NULL;
	}

	int n = matrix_getnumrows(_self);
	matrix* a = matrix_create_square(n);
	float** y = matrix_getdata(a);

	if (n == 1) {
		y[0][0] = 1;
		return a;
	}

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			matrix* cof = matrix_cofactor(_self, i, j);
			if (cof) {
				int sign = ((i + j) % 2 == 0) ? 1 : -1;
				y[j][i] = sign * matrix_determinant(cof);
				matrix_destroy(cof);
			}
		}
	}
	return a;
}

matrix * matrix_inverse(matrix * _self) {
	if (!matrix_issquare(_self)) {
		return NULL;
	}

	float det = matrix_determinant(_self);
	if (det == 0) {
		return NULL;
	}

	matrix* a = matrix_adjoint(_self);
	float f = 1 / det;
	matrix_scale(a, f);
	return a;
}

void matrix_print(matrix * _self) {
	printf("[");
	for (int i = 0; i < _self->nrows; ++i) {
		printf("[");
		for (int j = 0; j < _self->ncols; ++j) {
			printf("%f", _self->x[i][j]);
			if (j < _self->ncols - 1) {
				printf(", ");
			}
		}
		printf("]");
	}
	printf("]");
}
