/*
 * bsagohelper.h
 *
 *  Created on: Dec 11, 2017
 *      Author: assertantawi
 */

#ifndef SRC_BSAGOHELPER_H_
#define SRC_BSAGOHELPER_H_

#include <stdbool.h>

extern int* makeIntArray(int _size);

extern int getIntArrayElement(int* _array, int _index);

extern void setIntArrayElement(int* _array, int _value, int _index);

extern void freeIntArray(int* _array);


extern int** makeIntMatrix(int _rows, int* _cols);

extern int** makeIntMatrixComplete(int _rows, int _cols);

extern int getIntMatrixElement(int** _matrix, int _i, int _j);

extern void setIntMatrixElement(int** _matrix, int _value, int _i, int _j);

extern void freeIntMatrix(int** _matrix, int _rows);


extern char** makeStringArray(int _size);

extern char* getStringArrayElement(char** _array, int _index);

extern void setStringArrayElement(char** _array, char* _value, int _index);

extern void freeStringArray(char** _array);


extern char*** makeStringMatrix(int _rows, int* _cols);

extern char*** makeStringMatrixComplete(int _rows, int _cols);

extern char* getStringMatrixElement(char*** _matrix, int _i, int _j);

extern void setStringMatrixElement(char*** _matrix, char* _value, int _i, int _j);

extern void freeStringMatrix(char*** _matrix, int _rows);


extern bool* makeBoolArray(int _size);

extern bool getBoolArrayElement(bool* _array, int _index);

extern void setBoolArrayElement(bool* _array, bool _value, int _index);

extern void freeBoolArray(bool* _array);


extern float* makeFloatArray(int _size);

extern float getFloatArrayElement(float* _array, int _index);

extern void setFloatArrayElement(float* _array, float _value, int _index);

extern void freeFloatArray(float* _array);


extern float** makeFloatMatrix(int _rows, int* _cols);

extern float** makeFloatMatrixComplete(int _rows, int _cols);

extern float getFloatMatrixElement(float** _array, int _i, int _j);

extern void setFloatMatrixElement(float** _array, float _value, int _i, int _j);

extern void freeFloatMatrix(float** _array, int _rows);


#endif /* SRC_BSAGOHELPER_H_ */
