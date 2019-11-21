/*
 * demandstats.h
 *
 *  Created on: Sep 26, 2019
 *      Author: tantawi
 */

#ifndef STATS_DEMANDSTATS_H_
#define STATS_DEMANDSTATS_H_

#include "../utils/array.h"
#include "../utils/matrix.h"
#include "../system/le.h"

#define DEMAND_STATS_SMOOTHING_FACTOR 0.01

struct s_demandstats;

/**
 * Demand statistics collector
 */
typedef struct s_demandstats demandstats;

/**
 * create a demand statistics collector
 * (default factor used if given value is non-positive)
 */
extern demandstats * demandstats_create(float _smoothingfactor);

/**
 * destroy the demand statistics collector
 */
extern void demandstats_destroy(demandstats * _self);

/**
 * add LE demand data to statistics collector
 */
extern void demandstats_add(demandstats * _self, le* _le);

/**
 * average LE demand data from statistics collector (NULL if none)
 */
extern array * demandstats_getavg(demandstats * _self);

/**
 * second moment of LE demand from statistics collector (NULL if none)
 */
extern matrix * demandstats_getavg2(demandstats * _self);


/**
 * toString()
 */
extern void demandstats_print(demandstats * _self);

#endif /* STATS_DEMANDSTATS_H_ */
