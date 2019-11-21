/*
 * demandstats.c
 *
 *  Created on: Sep 26, 2019
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "demandstats.h"

/**
 * Demand statistics collector
 */
struct s_demandstats {

	/** factor used when adding new samples */
	float smoothingfactor;

	/** average demand */
	array* demandavg;

	/** squared average demand */
	matrix* demandavg2;

	/** sample count */
	int demandcount;
};

demandstats * demandstats_create(float _smoothingfactor) {
	demandstats * ds = (void *) malloc(sizeof(demandstats));
	assert(ds);

	if (_smoothingfactor > 0) {
		ds->smoothingfactor = _smoothingfactor;
	} else {
		ds->smoothingfactor = DEMAND_STATS_SMOOTHING_FACTOR;
	}

	ds->demandavg = NULL;
	ds->demandavg2 = NULL;
	ds->demandcount = 0;

	return ds;
}

void demandstats_destroy(demandstats * _self) {
	if (_self) {

		if (_self->demandavg) {
			array_destroy(_self->demandavg);
			_self->demandavg = NULL;
		}

		if (_self->demandavg2) {
			matrix_destroy(_self->demandavg2);
			_self->demandavg2 = NULL;
		}

		free(_self);
	}
}

void demandstats_add(demandstats * _self, le* _le) {
	int nres = le_getnumdemand(_le);
	bool nostats = (!_self->demandavg || !_self->demandavg2);

	if (nostats) {
		_self->demandavg = array_create(nres);
		_self->demandavg2 = matrix_create_square(nres);
	}

	float* avg = array_getdata(_self->demandavg);
	float** avg2 = matrix_getdata(_self->demandavg2);

	int countmax = (int) (1 / _self->smoothingfactor);
	if (countmax <= 0) {
		countmax = 1;
	}

	int* di = le_getdemand(_le);

	if (_self->demandcount < countmax) {
		_self->demandcount++;
	}
	float factor = 1 / ((float) _self->demandcount);

	float* old = malloc(nres * sizeof(float));
	assert(old);
	for (int j = 0; j < nres; ++j) {
		old[j] = avg[j];
	}

	if (nostats) {
		for (int j = 0; j < nres; ++j) {
			avg[j] = di[j];
			avg2[j][j] = di[j] * di[j];
			for (int k = 0; k < j; ++k) {
				avg2[k][j] = di[k] * di[j];
				avg2[j][k] = avg2[k][j];
			}
		}
		nostats = false;
	} else {
		for (int j = 0; j < nres; ++j) {
			avg[j] = (1 - factor) * avg[j] + factor * di[j];
			avg2[j][j] = (1 - factor) * avg2[j][j] + factor * di[j] * di[j];
			for (int k = 0; k < j; ++k) {
				avg2[k][j] = (1 - factor) * avg2[k][j] + factor * di[k] * di[j];
				avg2[j][k] = avg2[k][j];
			}
		}
	}

	free(old);
}

array * demandstats_getavg(demandstats * _self) {
	return _self->demandavg;
}

matrix * demandstats_getavg2(demandstats * _self) {
	return _self->demandavg2;
}


void demandstats_print(demandstats * _self) {
	printf("DemandStats:\n");
	printf("smoothingfactor=%f; ", _self->smoothingfactor);
	printf("demandcount=%d; ", _self->demandcount);
	printf("\n");

	printf("DemandAvg=");
	if (_self->demandavg) {
		array_print(_self->demandavg);
	} else {
		printf("NULL");
	}
	printf("\n");

	printf("DemandAvg2=");
	if (_self->demandavg2) {
		matrix_print(_self->demandavg2);
	} else {
		printf("NULL");
	}
	printf("\n");

	printf("\n");
}
