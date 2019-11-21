/*
 * printhelper.h
 *
 *  Created on: Feb 8, 2014
 *      Author: assertantawi
 */

#ifndef PRINTHELPER_H_
#define PRINTHELPER_H_

#include <stdbool.h>

/**
 * printing helper functions
 */

extern void printhelper_printvectorint(char* name, int* x, int len);

extern void printhelper_printvectorfloat(char* name, float* x, int len);

extern void printhelper_printvectorstring(char* name, char** x, int len);

extern void printhelper_printvectorbool(char* name, bool* x, int len);

extern void printhelper_printmatrixint(char* name, int** p, int nrows, int ncols);

extern void printhelper_printmatrixfloat(char* name, float** p, int nrows, int ncols);

extern void printhelper_printmatrixstring(char* name, char*** p, int nrows, int ncols);

extern void printhelper_printmatrixbool(char* name, bool** p, int nrows, int ncols);

extern void printhelper_printint(int* p);

extern void printhelper_printfloat(float* p);

extern void printhelper_printstring(char** p);

extern void printhelper_printbool(bool* p);

#endif /* PRINTHELPER_H_ */
