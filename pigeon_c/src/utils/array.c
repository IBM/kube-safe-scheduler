/*
 * array.c
 *
 *  Created on: Nov 2, 2018
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "array.h"

/**
 * An array of numbers
 */
struct s_array {
	/** number of elements in the array */
	int size;
	/** array of elements */
	float *x;
	/** allocated space */
	bool allocated;
};


array * array_create(int _n) {
	float* x = (float*) calloc(_n, sizeof(float));
	assert(x);

	array* a = array_createfromdata(_n, x);
	a->allocated = true;
	return a;
}

array * array_createfromdata(int _n, float * _data) {
	array * a = (void *) malloc(sizeof(array));
	assert(a);
	a->allocated = false;

	if (_n <= 0) {
		a->size = 0;
		a->x = NULL;
		return a;
	}

	a->size = _n;
	a->x = _data;

	return a;
}

void array_destroy(array * _self) {
	if (_self) {
		if (_self->allocated && _self->x) {
			free(_self->x);
			_self->x = NULL;
		}

		free(_self);
	}
}

int array_getlength(array * _self){
	return _self->size;
}

float* array_getdata(array * _self){
	return _self->x;
}

float array_getelement(array * _self, int _i){
	if (_i < 0 || _i >= _self->size) {
		return 0;
	}
	return _self->x[_i];
}

void array_setelement(array * _self, int _i, float _value) {
	if (_i >= 0 && _i < _self->size) {
		_self->x[_i] = _value;
	}
}

void array_set(array * _self, float _value){
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] = _value;
	}
}

array * array_clone(array * _self){
	array* other = array_create(_self->size);
	array_copy(other, _self);
	return other;
}

double array_sum(array * _self){
	double sum = 0;
	for (int i = 0; i < _self->size; i++) {
		sum += _self->x[i];
	}
	return sum;
}

double array_sumN(array * _self, int _n) {
	double sum = 0;
	for (int i = 0; i < _n && i < _self->size; i++) {
		sum += _self->x[i];
	}
	return sum;
}

void array_scale(array * _self, float _value){
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] *= _value;
	}
}

void array_shift(array * _self, float _value){
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] += _value;
	}
}

int array_minindex(array * _self) {
	if (_self->size <= 0) {
		return -1;
	}
	int minindex = 0;
	float minVal = _self->x[0];
	for (int i = 1; i < _self->size; i++) {
		if (_self->x[i] < minVal) {
			minindex = i;
			minVal = _self->x[i];
		}
	}
	return minindex;
}

int array_maxindex(array * _self) {
	if (_self->size <= 0) {
		return -1;
	}
	int maxindex = 0;
	float maxVal = _self->x[0];
	for (int i = 1; i < _self->size; i++) {
		if (_self->x[i] > maxVal) {
			maxindex = i;
			maxVal = _self->x[i];
		}
	}
	return maxindex;
}


bool array_isequal(array * _self, array * _other) {
	if (array_getlength(_other) != _self->size) {
		return false;
	}
	float* y = array_getdata(_other);
	for (int i = 0; i < _self->size; i++) {
		if (_self->x[i] != y[i]) {
			return false;
		}
	}
	return true;
}

void array_copy(array * _self, array * _other) {
	if (array_getlength(_other) != _self->size) {
		return;
	}
	float* y = array_getdata(_other);
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] = y[i];
	}
}

void array_add(array * _self, array * _other){
	if (array_getlength(_other) != _self->size) {
		return;
	}
	float* y = array_getdata(_other);
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] += y[i];
	}
}

void array_multiply(array * _self, array * _other){
	if (array_getlength(_other) != _self->size) {
		return;
	}
	float* y = array_getdata(_other);
	for (int i = 0; i < _self->size; i++) {
		_self->x[i] *= y[i];
	}
}

bool array_normalize(array * _self) {
	double sum = array_sum(_self);
	if (sum != 0) {
		float div = 1 / sum;
		array_scale(_self, div);
	}
	return sum != 0;
}

double array_average(array * _self) {
	double avg = array_sum(_self);
	if (_self->size > 0) {
		avg /= _self->size;
	}
	return avg;
}

double array_stddev(array * _self) {
	if (_self->size <= 0) {
		return 0;
	}
	double mean = 0;
	double mean2 = 0;
	for (int i = 0; i < _self->size; ++i) {
		mean += _self->x[i];
		mean2 += _self->x[i] * _self->x[i];
	}
	mean /= _self->size;
	mean2 /= _self->size;
	double var = mean2 - (mean * mean);
	var = fmax(var, 0);
	return sqrt(var);
}

void array_print(array * _self) {
	printf("[");
	for (int i = 0; i < _self->size; ++i) {
		printf("%f", _self->x[i]);
		if (i < _self->size - 1) {
			printf(", ");
		}
	}
	printf("]");
}
