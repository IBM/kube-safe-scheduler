/*
 * defs.c
 *
 *  Created on: Apr 15, 2015
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

char* defs_objtypeTostring(objective_type _objtype) {
	switch (_objtype) {
		case LOAD_BALANCE:
			return "LOAD_BALANCE";
		case CONSOLIDATE:
			return "CONSOLIDATE";
		case A_BINPACK:
			return "A_BINPACK";
		default:
			return "A_BINPACK";
	}
}

objective_type defs_objtypeFromstring(char* _s) {
	if (!_s) {
		return A_BINPACK;
	}
	if (strcmp(_s, "LOAD_BALANCE") == 0) {
		return LOAD_BALANCE;
	}
	if (strcmp(_s, "CONSOLIDATE") == 0) {
		return CONSOLIDATE;
	}
	if (strcmp(_s, "A_BINPACK") == 0) {
		return A_BINPACK;
	}

	return A_BINPACK;
}
