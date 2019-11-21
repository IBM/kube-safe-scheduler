/*
 * defs.h
 *
 *  Created on: Feb 10, 2014
 *      Author: assertantawi
 */

/**
 * General definitions
 */

#ifndef DEFS_H_
#define DEFS_H_


/** types of objective functions */
typedef enum {
	LOAD_BALANCE,
	CONSOLIDATE,
	MAX_LEVEL_RETURN,
	ZERO_SYSTEM,
	ZERO_BALANCE,
	RES_TOPOLOGY,
	MULTI_BALANCE,
	A_BINPACK,
	FIT_PACK,
	FIT_SPREAD,
	FIT_PACK_BALANCE,
	FIT_SPREAD_BALANCE,
	NETWORK,
	LOAD_BALANCE_NETWORK
} objective_type;


extern char* defs_objtypeTostring(objective_type _objtype);

extern objective_type defs_objtypeFromstring(char* _s);

#endif /* DEFS_H_ */
