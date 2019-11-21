/*
 * mathhelper.c
 *
 *  Created on: Feb 11, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "mathhelper.h"

bool mathhelper_arePracticallyEqual(float _newValue, float _oldValue,
		float relativeErrorTolerance) {

	float relativeError =
			(_oldValue != 0) ? ((_newValue - _oldValue) / _oldValue) : _newValue;
	return fabs(relativeError) <= relativeErrorTolerance;
}

bool mathhelper_normalizearray(float* _x, int _n) {
	double sum = 0;
	for (int i = 0; i < _n; i++) {
		sum += _x[i];
	}
	if (sum != 0) {
		for (int i = 0; i < _n; i++) {
			_x[i] /= sum;
		}
	}
	return sum != 0;
}

void mathhelper_addarray(float* _y, float* _x, int _n) {
	for (int i = 0; i < _n; i++) {
		_y[i] += _x[i];
	}
}

void mathhelper_multiplyarray(float* _y, float* _x, int _n) {
	for (int i = 0; i < _n; i++) {
		_y[i] *= _x[i];
	}
}

double mathhelper_sumarray(float* _x, int _n) {
	double sum = 0;
	for (int i = 0; i < _n; ++i) {
		sum += _x[i];
	}
	return sum;
}

void mathhelper_setarray(float* _x, int _n, float _v) {
	for (int i = 0; i < _n; i++) {
		_x[i] = _v;
	}
}

void mathhelper_scalearray(float* _x, int _n, float _v) {
	for (int i = 0; i < _n; i++) {
		_x[i] *= _v;
	}
}

void mathhelper_shiftarray(float* _x, int _n, float _v) {
	for (int i = 0; i < _n; i++) {
		_x[i] += _v;
	}
}

float* mathhelper_copyarray(float* _x, int _n) {
	float* y = (float*) malloc(_n * sizeof(float));
	assert(y);
	for (int i = 0; i < _n; ++i) {
		y[i] = _x[i];
	}
	return y;
}

void mathhelper_array_add_scalar(double* _x, int _n, double _v) {
	for (int i = 0; i < _n; i++) {
		_x[i] += _v;
	}
}

double mathhelper_average(float* _x, int _n) {
	double avg = mathhelper_sumarray(_x, _n);
	avg = (_n > 0) ? avg / _n : 0;
	return avg;
}

double mathhelper_stddev(float* _x, int _n) {
	if (_n <= 0) {
		return 0;
	}
	double mean = 0;
	double mean2 = 0;
	for (int i = 0; i < _n; ++i) {
		mean += _x[i];
		mean2 += _x[i] * _x[i];
	}
	mean /= _n;
	mean2 /= _n;
	double var = mean2 - (mean * mean);
	var = fmax(var, 0);
	return sqrt(var);
}

int mathhelper_floatcomp(const void* _elem1, const void* _elem2) {
	if (*(const float*) _elem1 < *(const float*) _elem2)
		return -1;
	return *(const float*) _elem1 > *(const float*) _elem2;
}


