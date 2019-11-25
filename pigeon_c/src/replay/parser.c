/*
 * parser.c
 *
 *  Created on: Aug 22, 2014
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "../utils/stringbuffer.h"


char* parser_nextkeyvaluepair(char* q, char* k, char* v, bool* issingleton) {
	char* next = NULL;
	int keylen = 0;
	int valuelen = 0;

	char* eq = strchr(q, '=');
	if (eq) {
		// get key
		char* f = q;
		//disregard leading blanks
		while (*f == ' ') {
			f++;
		}
		keylen = eq - f;
		strncpy(k, f, keylen);

		//get value
		char* valbegin = eq + 1;
		if (*valbegin == '{') {
			// value is not a singleton
			*issingleton = false;
			valbegin++;
			// count matching brackets { }
			int count = 1;
			char* c = valbegin;
			char* valend = NULL;
			while (!(*c == '\0' || *c == '\n')) {
				if (*c == '{') {
					count++;
				}
				if (*c == '}') {
					count--;
					if (count == 0) {
						valend = c;
						break;
					}
				}
				c++;
			}
			if (valend) {
				valuelen = valend - valbegin;
				strncpy(v, valbegin, valuelen);
				//skip the closing bracket }
				valend++;
				if (!(*valend == '\0' || *valend == '\n')) {
					next = valend + 1;
				}
			}
		}

		else {
			// value is a singleton
			*issingleton = true;
			char* ct = ";}\n";
			char* valend = strpbrk(valbegin, ct);
			if (!valend) {
				valend = valbegin + strlen(valbegin);
			}
			valuelen = valend - valbegin;
			strncpy(v, valbegin, valuelen);
			if (!(*valend == '\0' || *valend == '\n')) {
				next = valend + 1;
			}
		}
	}

	k[keylen] = '\0';
	v[valuelen] = '\0';
	return next;
}

int parser_decode_int(char * s) {
	int value = 0;
	if (s) {
		sscanf(s, "%d", &value);
	}
	return value;
}

int** parser_decode_intarray(char * s, int nrows, int ncols) {
	int ** value = (int**) malloc(nrows * sizeof(int*));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (int*) calloc(ncols, sizeof(int));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		sscanf(tok, "%d", &value[i][j]);
		j++;
		if (j == ncols) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

float parser_decode_float(char * s) {
	float value = 0;
	if (s) {
		sscanf(s, "%f", &value);
	}
	return value;
}

float** parser_decode_floatarray(char * s, int nrows, int ncols) {
	float ** value = (float**) malloc(nrows * sizeof(float*));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (float*) calloc(ncols, sizeof(float));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		sscanf(tok, "%f", &value[i][j]);
		j++;
		if (j == ncols) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

float** parser_decode_floatarrayvariable(char * s, int nrows, int * ncols) {
	float ** value = (float**) malloc(nrows * sizeof(float*));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (float*) calloc(ncols[i], sizeof(float));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		sscanf(tok, "%f", &value[i][j]);
		j++;
		if (j == ncols[i]) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

char* parser_decode_string(char * s) {
	char* value = (char*) malloc((strlen(s) + 1) * sizeof(char));
	strcpy(value, s);
	return value;
}

char*** parser_decode_stringarray(char * s, int nrows, int ncols) {
	char *** value = (char***) malloc(nrows * sizeof(char**));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (char**) calloc(ncols, sizeof(char*));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		value[i][j] = (char*) malloc((strlen(tok) + 1) * sizeof(char));
		sscanf(tok, "%s", value[i][j]);
		j++;
		if (j == ncols) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

char*** parser_decode_stringarrayvariable(char * s, int nrows, int * ncols) {
	char *** value = (char***) malloc(nrows * sizeof(char**));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (char**) calloc(ncols[i], sizeof(char*));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		value[i][j] = (char*) malloc((strlen(tok) + 1) * sizeof(char));
		sscanf(tok, "%s", value[i][j]);
		j++;
		if (j == ncols[i]) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

bool parser_decode_bool(char * s) {
	return (strcmp(s, "true") == 0 || strcmp(s, "TRUE") == 0
			|| strcmp(s, "1") == 0);
}

bool** parser_decode_boolarray(char * s, int nrows, int ncols) {
	bool ** value = (bool**) malloc(nrows * sizeof(bool*));
	assert(value);

	for (int i = 0; i < nrows; i++) {
		value[i] = (bool*) calloc(ncols, sizeof(bool));
		assert(value[i]);
	}

	if (!s) {
		return value;
	}

	// create a copy of the input string
	char* cs = (char*) malloc((strlen(s) + 1) * sizeof(char));
	assert(cs);
	strcpy(cs, s);

	char* tok = strtok(cs, BRACKETS);

	int i = 0;
	int j = 0;
	while(tok) {
		value[i][j] = parser_decode_bool(tok);
		j++;
		if (j == ncols) {
			j = 0;
			i++;
			if (i == nrows) {
				i = 0;
			}
		}
		tok = strtok(NULL, BRACKETS);
	}

	free(cs);
	return value;
}

char* parser_encode_int(char * key, int val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=", false);

	char valstr[16];
	sprintf(valstr, "%d", val);
	stringbuffer_append(sb, valstr, false);

	stringbuffer_append(sb, ";", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_intarray(char * key, int num, int* val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	char valstr[16];
	for (int i = 0; i < num; ++i) {
		sprintf(valstr, "%d", val[i]);
		stringbuffer_append(sb, valstr, false);
		if (i < num - 1) {
			stringbuffer_append(sb, ",", false);
		}
	}

	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_intmatrix(char * key, int nrows, int ncols, int** val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	char valstr[32];
	for (int i = 0; i < nrows; ++i) {
		stringbuffer_append(sb, "[", false);
		for (int j = 0; j < ncols; ++j) {
			sprintf(valstr, "%d", val[i][j]);
			stringbuffer_append(sb, valstr, false);
			if (j < ncols - 1) {
				stringbuffer_append(sb, ",", false);
			}
		}
		stringbuffer_append(sb, "]", false);
	}
	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}
char* parser_encode_float(char * key, float val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=", false);

	char valstr[16];
	sprintf(valstr, "%f", val);
	stringbuffer_append(sb, valstr, false);

	stringbuffer_append(sb, ";", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_floatarray(char * key, int num, float* val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	char valstr[32];
	for (int i = 0; i < num; ++i) {
		sprintf(valstr, "%f", val[i]);
		stringbuffer_append(sb, valstr, false);
		if (i < num - 1) {
			stringbuffer_append(sb, ",", false);
		}
	}

	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_floatmatrix(char * key, int nrows, int ncols, float** val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	char valstr[32];
	for (int i = 0; i < nrows; ++i) {
		stringbuffer_append(sb, "[", false);
		for (int j = 0; j < ncols; ++j) {
			sprintf(valstr, "%f", val[i][j]);
			stringbuffer_append(sb, valstr, false);
			if (j < ncols - 1) {
				stringbuffer_append(sb, ",", false);
			}
		}
		stringbuffer_append(sb, "]", false);
	}
	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_bool(char * key, bool val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=", false);

	char valstr[16];
	sprintf(valstr, "%d", val);
	stringbuffer_append(sb, valstr, false);

	stringbuffer_append(sb, ";", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_boolarray(char * key, int num, bool* val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	char valstr[16];
	for (int i = 0; i < num; ++i) {
		sprintf(valstr, "%d", val[i]);
		stringbuffer_append(sb, valstr, false);
		if (i < num - 1) {
			stringbuffer_append(sb, ",", false);
		}
	}
	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_string(char * key, char* val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=", false);
	stringbuffer_append(sb, val, false);
	stringbuffer_append(sb, ";", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_stringarray(char * key, int num, char** val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	for (int i = 0; i < num; ++i) {
		stringbuffer_append(sb, val[i], false);
		if (i < num - 1) {
			stringbuffer_append(sb, ",", false);
		}
	}
	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}

char* parser_encode_stringmatrix(char * key, int nrows, int ncols, char*** val) {
	stringbuffer* sb = stringbuffer_create();

	stringbuffer_append(sb, key, false);
	stringbuffer_append(sb, "=[", false);

	for (int i = 0; i < nrows; ++i) {
		stringbuffer_append(sb, "[", false);
		for (int j = 0; j < ncols; ++j) {
			stringbuffer_append(sb, val[i][j], false);
			if (j < ncols - 1) {
				stringbuffer_append(sb, ",", false);
			}
		}
		stringbuffer_append(sb, "]", false);
	}
	stringbuffer_append(sb, "];", false);

	char* t = stringbuffer_getstring(sb);
	stringbuffer_reset(sb);
	stringbuffer_destroy(sb);
	return t;
}
