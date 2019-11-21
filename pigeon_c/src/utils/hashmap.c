/*
 * hashmap.c
 *
 *  Created on: Feb 15, 2014
 *      Author: assertantawi
 */

/**
 * sources:
 * http://www2.informatik.hu-berlin.de/~weber/slipOff/hashmap_c.html
 *
 * http://www.w3.org/2001/06/blindfold/api/hashmap_8c-source.html
 *
 * modified
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "hashmap.h"

/*--- HashPJW ---------------------------------------------------
*  An adaptation of Peter Weinberger's (PJW) generic hashing
*  algorithm based on Allen Holub's version. Accepts a pointer
*  to a datum to be hashed and returns an unsigned integer.
*  Modified by sandro to include datum_end
*  Taken from http://www.ddj.com/articles/1996/9604/9604b/9604b.htm?topic=algorithms
*-------------------------------------------------------------*/
#define BITS_IN_int     ( sizeof(long) * CHAR_BIT )
#define THREE_QUARTERS  ((long) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH      ((long) (BITS_IN_int / 8))
#define HIGH_BITS       ( ~((unsigned long)(~0) >> ONE_EIGHTH ))

unsigned long hashfunction(char* _datum) {
	unsigned long hash_value, i;
	char *datum_end = &(_datum [strlen(_datum)]);
	for (hash_value = 0; _datum < datum_end; ++_datum) {
		hash_value = (hash_value << ONE_EIGHTH) + *_datum;
		if ((i = hash_value & HIGH_BITS) != 0)
			hash_value = (hash_value ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
	}
	return hash_value;
}


/* this should be prime */
#define TABLE_STARTSIZE 1021

#define ACTIVE 1

/**
 * entry element in the hash table
 */
typedef struct {
	/** value of the entry, pointer to data associated with this entry */
	void* data;
	/** flags for the entry, used to note activity */
	int flags;
	/** key of the entry */
	char* keystring;
} hEntry;

/** Hashmap structure */
struct s_hashmap {
	hEntry* table;
	long size, count;
};

static unsigned long isPrime(unsigned long _val) {
	int i, p, exp, a;

	for (i = 9; i--;) {
		a = (rand() % (_val - 4)) + 2;
		p = 1;
		exp = _val - 1;
		while (exp) {
			if (exp & 1)
				p = (p * a) % _val;

			a = (a * a) % _val;
			exp >>= 1;
		}

		if (p != 1)
			return 0;
	}

	return 1;
}

static int findPrimeGreaterThan(int _val) {
	if (_val & 1)
		_val += 2;
	else
		_val++;

	while (!isPrime(_val))
		_val += 2;

	return _val;
}

static void rehash(hashmap* _hm) {
	long size = _hm->size;
	hEntry* table = _hm->table;

	_hm->size = findPrimeGreaterThan(size << 1);
	_hm->table = (hEntry*) calloc(sizeof(hEntry), _hm->size);
	_hm->count = 0;

	while (--size >= 0)
		if (table[size].flags == ACTIVE)
			hashmap_insert(_hm, table[size].data, table[size].keystring);

	free(table);
}

hashmap* hashmap_create(int _startsize) {
	hashmap* hm = (hashmap*) malloc(sizeof(hashmap));

	if (!_startsize)
		_startsize = TABLE_STARTSIZE;
	else
		_startsize = findPrimeGreaterThan(_startsize - 2);

	hm->table = (hEntry*) calloc(sizeof(hEntry), _startsize);
	hm->size = _startsize;
	hm->count = 0;

	return hm;
}

void hashmap_destroy(hashmap* _self) {
	if (_self) {
		free(_self->table);
		free(_self);
	}
}

void hashmap_insert(hashmap* _self, const void* _data, char* _keystring) {
	unsigned long key = hashfunction(_keystring);
	long index, i, step;

	if (_self->size <= _self->count)
		rehash(_self);

	do {
		index = key % _self->size;
		step = (key % (_self->size - 2)) + 1;

		for (i = 0; i < _self->size; i++) {
			if (_self->table[index].flags & ACTIVE) {

				if (_self->table[index].keystring
					&& strcmp(_self->table[index].keystring, _keystring) == 0) {
					_self->table[index].data = (void*) _data;
					return;
				}
			} else {
				_self->table[index].flags |= ACTIVE;
				_self->table[index].data = (void*) _data;
				_self->table[index].keystring = _keystring;
				++_self->count;
				return;
			}

			index = (index + step) % _self->size;
		}

		/* it should not be possible that we EVER come this far, but unfortunately
		 not every generated prime number is prime (Carmichael numbers...) */
		rehash(_self);
	} while (1);
}

void* hashmap_remove(hashmap* _self, char* _keystring) {
	unsigned long key = hashfunction(_keystring);
	long index, i, step;

	index = key % _self->size;
	step = (key % (_self->size - 2)) + 1;

	for (i = 0; i < _self->size; i++) {
		if (_self->table[index].data) {

			if (_self->table[index].keystring
					&& strcmp(_self->table[index].keystring, _keystring) == 0) {
				if (_self->table[index].flags & ACTIVE) {
					_self->table[index].flags &= ~ACTIVE;
					--_self->count;
					return _self->table[index].data;
				} else
					/* in, but not active (i.e. deleted) */
					return 0;
			}
		} else
			/* found an empty place (can't be in) */
			return 0;

		index = (index + step) % _self->size;
	}
	/* everything searched through, but not in */
	return 0;
}

void* hashmap_get(hashmap* _self, char* _keystring) {
	unsigned long key = hashfunction(_keystring);
	if (_self->count) {
		long index, i, step;
		index = key % _self->size;
		step = (key % (_self->size - 2)) + 1;

		for (i = 0; i < _self->size; i++) {
			if (_self->table[index].keystring
					&& strcmp(_self->table[index].keystring, _keystring) == 0) {
				if (_self->table[index].flags & ACTIVE)
				return _self->table[index].data;
				break;
			} else if (!_self->table[index].data)
			break;

			index = (index + step) % _self->size;
		}
	}

	return 0;
}

long hashmap_count(hashmap* _self) {
	return _self->count;
}

extern vector* hashmap_getvalues(hashmap* _self) {
	vector* v = vector_create();
	if (_self->count) {
		for (int i = 0; i < _self->size; i++) {
			if (_self->table[i].flags & ACTIVE) {
				void* data_element = _self->table[i].data;
				if (data_element) {
					vector_append(v, data_element);
				}
			}
		}
	}
	return v;
}

extern vector* hashmap_getkeys(hashmap* _self) {
	vector* v = vector_create();
	if (_self->count) {
		for (int i = 0; i < _self->size; i++) {
			if (_self->table[i].flags & ACTIVE) {
				vector_append(v, _self->table[i].keystring);
			}
		}
	}
	return v;
}

void hashmap_print(hashmap* _self, void (*data_print)(void *)) {
	printf("[ ");
	if (_self->count) {
		int num = 0;
		for (int i = 0; i < _self->size; i++) {
			if (_self->table[i].flags & ACTIVE) {
				void* data_element = _self->table[i].data;
				if (data_element) {
					printf("%s=>", _self->table[i].keystring);
					(*data_print)(data_element);
					if (num < _self->count - 1) {
						printf(", ");
					}
					num++;
				}
			}
		}
	}
	printf(" ]");
}

void hashmap_printbrief(hashmap* _self, void (*data_print)(void *)) {
	if (_self->count) {
		int num = 0;
		for (int i = 0; i < _self->size; i++) {
			if (_self->table[i].flags & ACTIVE) {
				void* data_element = _self->table[i].data;
				if (data_element) {
					printf("%s=>", _self->table[i].keystring);
					(*data_print)(data_element);
					if (num < _self->count - 1) {
						printf(";");
					}
					num++;
				}
			}
		}
	}
}

