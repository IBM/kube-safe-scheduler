/*
 * stateupdater.h
 *
 *  Created on: May 31, 2018
 *      Author: tantawi
 */

#ifndef SYSTEM_STATEUPDATER_H_
#define SYSTEM_STATEUPDATER_H_

#include <stdbool.h>

#include "cloudsystem.h"

struct s_stateupdater;

/**
 * An updater to the state of the cloud system model given the observed state.
 * System model is assumed to be already built (could be empty)
 *
 * Updates are communicated in a session delimited by start() and stop().
 * Observations about all PEs and all LEs in the system are to be provided
 * by update(). The order is important: update PEs first then LEs.
 * start() and stop() should be called only once each in a session.
 *
 * When updating an LE host placement change, all LE removals are done first,
 * then all LE placements are done in the end of the update session.
 * This does not result in overflowing resources while effecting the changes.
 */
typedef struct s_stateupdater stateupdater;

/**
 * create a state updater
 */
extern stateupdater * stateupdater_create(cloudsystem* _cs);

/**
 * destroy the state updater
 */
extern void stateupdater_destroy(stateupdater* _self);

/**
 * start an update session
 * (given an observation generation sequence number)
 * returns false in case update is not necessary; true otherwise
 */
extern bool stateupdater_start(stateupdater* _self, long _generation);

/**
 * finish the update session and effect the updates into the cloud system model
 */
extern void stateupdater_stop(stateupdater* _self);

/**
 * update a PE given its observation
 * (if _usage NULL, do not update usage, otherwise force update))
 */
extern void stateupdater_updatePE(stateupdater* _self, char* _id,
		int* _capacity, bool* _ovf, int* _usage, int _length);

/**
 * update an LE given its observation
 */
extern void stateupdater_updateLE(stateupdater* _self, char* _id,
		char* _hostID, int* _demand, int _length);

/**
 * get the updated cloud system model
 */
extern cloudsystem* stateupdater_getupdated(stateupdater* _self);

/**
 * toString()
 */
extern void stateupdater_print(stateupdater * _self);

#endif /* SYSTEM_STATEUPDATER_H_ */
