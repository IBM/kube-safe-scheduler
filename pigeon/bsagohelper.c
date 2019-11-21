/*
 * bsagohelper.c
 *
 *  Created on: Dec 11, 2017
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bsagohelper.h"

int* makeIntArray(int _size) {
	int* array = (int*) calloc(_size, sizeof(int));
	assert(array);
	return array;
}

int getIntArrayElement(int* _array, int _index) {
	return _array[_index];
}

void setIntArrayElement(int* _array, int _value, int _index) {
	_array[_index] = _value;
}

void freeIntArray(int* _array) {
	free(_array);
}


int** makeIntMatrix(int _rows, int* _cols) {
	int** matrix = (int**) calloc(_rows, sizeof(int*));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (int*) calloc(_cols[i], sizeof(int));
		assert(matrix[i]);
	}
	return matrix;
}

int** makeIntMatrixComplete(int _rows, int _cols) {
	int** matrix = (int**) calloc(_rows, sizeof(int*));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (int*) calloc(_cols, sizeof(int));
		assert(matrix[i]);
	}
	return matrix;
}

int getIntMatrixElement(int** _matrix, int _i, int _j) {
	return _matrix[_i][_j];
}

void setIntMatrixElement(int** _matrix, int _value, int _i, int _j) {
	_matrix[_i][_j] = _value;
}

void freeIntMatrix(int** _matrix, int _rows) {
	for (int i = 0; i < _rows; i++) {
		free(_matrix[i]);
	}
	free(_matrix);
}


char** makeStringArray(int _size) {
	char** array = (char**) calloc(_size, sizeof(char*));
	assert(array);
	return array;
}

char* getStringArrayElement(char** _array, int _index) {
	return _array[_index];
}

void setStringArrayElement(char** _array, char* _value, int _index) {
	_array[_index] = _value;
}

void freeStringArray(char** _array) {
	free(_array);
}


char*** makeStringMatrix(int _rows, int* _cols) {
	char*** matrix = (char***) calloc(_rows, sizeof(char**));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (char**) calloc(_cols[i], sizeof(char*));
		assert(matrix[i]);
	}
	return matrix;
}

char*** makeStringMatrixComplete(int _rows, int _cols) {
	char*** matrix = (char***) calloc(_rows, sizeof(char**));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (char**) calloc(_cols, sizeof(char*));
		assert(matrix[i]);
	}
	return matrix;
}

char* getStringMatrixElement(char*** _matrix, int _i, int _j) {
	return _matrix[_i][_j];
}

void setStringMatrixElement(char*** _matrix, char* _value, int _i, int _j) {
	_matrix[_i][_j] = _value;
}

void freeStringMatrix(char*** _matrix, int _rows) {
	for (int i = 0; i < _rows; i++) {
		free(_matrix[i]);
	}
	free(_matrix);
}


bool* makeBoolArray(int _size) {
	bool* array = (bool*) calloc(_size, sizeof(bool));
	assert(array);
	return array;
}

bool getBoolArrayElement(bool* _array, int _index) {
	return _array[_index];
}

void setBoolArrayElement(bool* _array, bool _value, int _index) {
	_array[_index] = _value;
}

void freeBoolArray(bool* _array) {
	free(_array);
}


float* makeFloatArray(int _size) {
	float* array = (float*) calloc(_size, sizeof(float));
	assert(array);
	return array;
}

float getFloatArrayElement(float* _array, int _index){
	return _array[_index];
}

void setFloatArrayElement(float* _array, float _value, int _index) {
	_array[_index] = _value;
}

void freeFloatArray(float* _array) {
	free(_array);
}


float** makeFloatMatrix(int _rows, int* _cols) {
	float** matrix = (float**) calloc(_rows, sizeof(float*));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (float*) calloc(_cols[i], sizeof(float));
		assert(matrix[i]);
	}
	return matrix;
}

float** makeFloatMatrixComplete(int _rows, int _cols) {
	float** matrix = (float**) calloc(_rows, sizeof(float*));
	assert(matrix);
	for (int i = 0; i < _rows; i++) {
		matrix[i] = (float*) calloc(_cols, sizeof(float));
		assert(matrix[i]);
	}
	return matrix;
}

float getFloatMatrixElement(float** _matrix, int _i, int _j) {
	return _matrix[_i][_j];
}

void setFloatMatrixElement(float** _matrix, float _value, int _i, int _j) {
	_matrix[_i][_j] = _value;
}

void freeFloatMatrix(float** _matrix, int _rows) {
	for (int i = 0; i < _rows; i++) {
		free(_matrix[i]);
	}
	free(_matrix);
}
