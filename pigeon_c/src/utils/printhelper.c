/*
 * printhelper.c
 *
 *  Created on: Feb 8, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>

#include "printhelper.h"


void printhelper_printvectorint(char* name, int* x, int len) {
	if (name == NULL || x == NULL) {
		return;
	}
	printf("%s=[", name);
	for (int i = 0; i < len; i++) {
		printf("%d", x[i]);
		if (i < len - 1) {
			printf(",");
		}
	}
	printf("]");
}

void printhelper_printvectorfloat(char* name, float* x, int len) {
	if (name == NULL || x == NULL) {
		return;
	}
	printf("%s=[", name);
	for (int i = 0; i < len; i++) {
		printf("%f", x[i]);
		if (i < len - 1) {
			printf(",");
		}
	}
	printf("]");
}

void printhelper_printvectorstring(char* name, char** x, int len) {
	if (name == NULL || x == NULL) {
		return;
	}
	printf("%s=[", name);
	for (int i = 0; i < len; i++) {
		printf("%s", x[i]);
		if (i < len - 1) {
			printf(",");
		}
	}
	printf("]");
}

void printhelper_printvectorbool(char* name, bool* x, int len) {
	if (name == NULL || x == NULL) {
		return;
	}
	printf("%s=[", name);
	for (int i = 0; i < len; i++) {
		printf("%d", x[i]);
		if (i < len - 1) {
			printf(",");
		}
	}
	printf("]");
}

void printhelper_printmatrixfloat(char* name, float** p, int nrows, int ncols) {
	if (name == NULL || p == NULL) {
		return;
	}
	printf("%s=[ ", name);
	for (int i = 0; i < nrows; i++) {
		printf("[ ");
		for (int j = 0; j < ncols; j++) {
			printf("%f ", p[i][j]);
		}
		printf("] ");
		if (ncols > 10) {
			printf("\n");
		}
	}
	printf("]");
}

void printhelper_printmatrixint(char* name, int** p, int nrows, int ncols) {
	if (name == NULL || p == NULL) {
		return;
	}
	printf("%s=[ ", name);
	for (int i = 0; i < nrows; i++) {
		printf("[ ");
		for (int j = 0; j < ncols; j++) {
			printf("%d ", p[i][j]);
		}
		printf("] ");
		if (ncols > 10) {
			printf("\n");
		}
	}
	printf("]");
}

void printhelper_printmatrixstring(char* name, char*** p, int nrows, int ncols) {
	if (name == NULL || p == NULL) {
		return;
	}
	printf("%s=[ ", name);
	for (int i = 0; i < nrows; i++) {
		printf("[ ");
		for (int j = 0; j < ncols; j++) {
			if(p[i][j]) {
				printf("%s ", p[i][j]);
			}
		}
		printf("] ");
		if (ncols > 10) {
			printf("\n");
		}
	}
	printf("]");
}

void printhelper_printmatrixbool(char* name, bool** p, int nrows, int ncols) {
	if (name == NULL || p == NULL) {
		return;
	}
	printf("%s=[ ", name);
	for (int i = 0; i < nrows; i++) {
		printf("[ ");
		for (int j = 0; j < ncols; j++) {
			printf("%d ", p[i][j]);
		}
		printf("] ");
		if (ncols > 10) {
			printf("\n");
		}
	}
	printf("]");
}

void printhelper_printint(int* p) {
	printf("%d", *p);
}

void printhelper_printfloat(float* p) {
	printf("%f", *p);
}

void printhelper_printstring(char** p) {
	printf("%s", *p);
}

void printhelper_printbool(bool* p) {
	printf("%d", *p);
}

