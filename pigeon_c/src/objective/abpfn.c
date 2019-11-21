/*
 * abpfn.c
 *
 *  Created on: Jan 12, 2016
 *      Author: assertantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../system/pe.h"
#include "../system/resource.h"
#include "../utils/mathhelper.h"
#include "../utils/matrix.h"

#include "abpfn.h"
#include "../stats/demandstats.h"


/**
 * An Adaptive Bin Packing (ABP) objective function
 */
struct s_abpfn {

};

abpfn * abpfn_create() {
	abpfn * fn = (void *) malloc(sizeof(abpfn));
	assert(fn);

	return fn;
}

void abpfn_destroy(void * _self) {
	if (_self) {

		free(_self);
	}
}

/**
 * compute the variation in system load
 * return covariance and average (and average capacity)
 * in supplied addresses
 */
bool abpnf_getvariation_system(objfn * _f, matrix** covp, array** avgp) {

	cloudsystem * cs = objfn_getcloudsystem(_f);
	if (!cs) {
		return false;
	}
	int n = cloudsystem_getnumPEs(cs);
	int m = cloudsystem_getnumresources(cs);
	if (n == 0 || m == 0) {
		return false;
	}

	/*
	 * collect system data
	 */
	matrix* sysM = matrix_create(m, n);
	float** u = matrix_getdata(sysM);

	vector * pes = cloudsystem_getPEs(cs);
	for (int i = 0; i < n; ++i) {
		pe * host = vector_get(pes, i);
		resource * r = pe_getresource(host);
		if (r) {
			float * util = resource_getutilization(r);
			for (int j = 0; j < m; ++j) {
				u[j][i] = util[j];
			}
			free(util);
		}
	}

	/*
	 * Calculate average and covariance of PE utilization
	 */
	*avgp = matrix_average(sysM);
	*covp = matrix_covariance(sysM);

	if (objfn_isprint(_f)) {
		printf("system matrix: ");
		matrix_print(sysM);
		printf("\n");

		printf("system average: ");
		array_print(*avgp);
		printf("\n");

		printf("system covariance: ");
		matrix_print(*covp);
		printf("\n");
	}

	matrix_destroy(sysM);
	return true;
}

/**
 * compute the variation in demand, given history data
 * return covariance and average in supplied addresses
 */
bool abpnf_getvariation_demand_history(objfn * _f, matrix** covp, array** avgp) {

	cloudsystem * cs = objfn_getcloudsystem(_f);
	if (!cs) {
		return false;
	}

	/*
	 * get demand history data and normalize
	 */
	demandstats* dstats = cloudsystem_getdemandstats(cs);
	if (!dstats) {
		return false;
	}
	array* mu = demandstats_getavg(dstats);
	matrix* mu2 = demandstats_getavg2(dstats);
	if (!mu || !mu2) {
		return false;
	}

	float* avgcap = cloudsystem_getaveragecapacity(cs);
	if (!avgcap) {
		return false;
	}

	array* munorm = array_clone(mu);
	matrix* mu2norm = matrix_clone(mu2);
	int p = matrix_getnumrows(mu2norm);

	float* a = array_getdata(munorm);
	float** b = matrix_getdata(mu2norm);
	for (int i = 0; i < p; ++i) {
		if (avgcap[i] > 0) {
			a[i] /= avgcap[i];
			for (int j = 0; j < p; ++j) {
				b[i][j] /= avgcap[i] * avgcap[j];
			}
		}
	}

	/*
	 * Calculate average and covariance of LE demand
	 */
	*avgp = munorm;
	*covp = matrix_covariance_from_stats(mu2norm, munorm);

	if (objfn_isprint(_f)) {
		printf("demand average: ");
		array_print(mu);
		printf("\n");
		printf("demand relative average : ");
		array_print(munorm);
		printf("\n");
		printf("demand average2: ");
		matrix_print(mu2);
		printf("\n");
		printf("demand relative average2: ");
		matrix_print(mu2norm);
		printf("\n");
		if (*covp) {
			printf("demand covariance: ");
			matrix_print(*covp);
			printf("\n");
		}
	}

	free(avgcap);
	matrix_destroy(mu2norm);

	if (!*covp) {
		array_destroy(munorm);
		*avgp = NULL;
		return false;
	}

	return true;
}

/**
 * compute the variation in demand, given current load
 * return covariance and average in supplied addresses
 */
bool abpnf_getvariation_demand_current(objfn * _f, matrix** covp, array** avgp) {

	cloudsystem * cs = objfn_getcloudsystem(_f);
	if (!cs) {
		return false;
	}

	int n = cloudsystem_getnumPEs(cs);
	int m = cloudsystem_getnumresources(cs);
	if (n == 0 || m == 0) {
		return false;
	}

	float* avgcap = cloudsystem_getaveragecapacity(cs);
	if (!avgcap) {
		return false;
	}

	// Collect total number of LEs in the system
	vector * pes = cloudsystem_getPEs(cs);
	int numles = 0;
	for (int i = 0; i < n; ++i) {
		pe * host = vector_get(pes, i);
		numles += vector_getsize(pe_gethostedLE(host));
	}

	if (numles == 0) {
		free(avgcap);
		return false;
	}

	matrix* demM = matrix_create(m, numles);
	float** d = matrix_getdata(demM);

	/*
	 * collect demand data and normalize
	 */
	int index = 0;
	for (int i = 0; i < n; ++i) {
		pe * host = vector_get(pes, i);
		vector* les = pe_gethostedLE(host);
		int num = vector_getsize(les);
		for (int k = 0; k < num; ++k) {
			le* lek = vector_get(les, k);
			int* ledemand = le_getdemand(lek);
			for (int j = 0; j < m; ++j) {
				d[j][index] =
						(avgcap[j] > 0) ? ledemand[j] / avgcap[j] : 1;
			}
			index++;
		}
	}

	/*
	 * Calculate average and covariance of LE demand
	 */
	*avgp = matrix_average(demM);
	*covp = matrix_covariance(demM);

	if (objfn_isprint(_f)) {
		printf("demand matrix: ");
		matrix_print(demM);
		printf("\n");

		printf("demand average: ");
		array_print(*avgp);
		printf("\n");

		printf("demand covariance: ");
		matrix_print(*covp);
		printf("\n");
	}

	free(avgcap);
	matrix_destroy(demM);
	return true;
}

/*
 * Comparison of system and demand variation
 */
double abpfn_diffvalue(objfn * _f, matrix* _cov_sys, array* _avg_sys,
		matrix* _cov_dem, array* _avg_dem) {

	double value = 0;
	matrix_cov_method cov_method = ALBERT_ZHANG;
	bool isprint = objfn_isprint(_f);

	if (ABPN_COMPARE_SCALAR == 1) {

		double covS = matrix_coefficient_variation_from_stats(_cov_sys,
				_avg_sys, cov_method);

		double covD = matrix_coefficient_variation_from_stats(_cov_dem,
				_avg_dem, cov_method);

		double diff = covS - covD;
		value = (diff >= 0) ? diff : -diff;

		if (isprint) {
			printf("covS=%f; covD=%f; \n", covS, covD);
		}

	} else {

		matrix* covS = matrix_cov2_matrix(_cov_sys, _avg_sys);
		matrix* covD = matrix_cov2_matrix(_cov_dem, _avg_dem);

		if (covS && covD) {
			double diff = 0;
			int p = matrix_getnumrows(covS);
			float** S = matrix_getdata(covS);
			float** D = matrix_getdata(covD);

			for (int i = 0; i < p; ++i) {
				for (int j = 0; j < p; ++j) {
					diff += pow((S[i][j] - D[i][j]), 2);
				}
			}
			value = sqrt(diff);

			if (isprint) {
				printf("covS matrix: ");
				matrix_print(covS);
				printf("\n");

				printf("covD matrix: ");
				matrix_print(covD);
				printf("\n");
			}
		}

		matrix_destroy(covS);
		matrix_destroy(covD);
	}

	return value;
}

float abpfn_getsystemvalue(objfn * _f) {

	bool isprint = objfn_isprint(_f);
	if (isprint) {
		printf("\n");
		printf("+++++ Begin abpfn_getsystemvalue(): +++++\n");
	}

	/*
	 * Coefficient of variation of system utilization
	 */
	matrix* cov_sys = NULL;
	array* avg_sys = NULL;
	bool ok_sys = abpnf_getvariation_system(_f, &cov_sys, &avg_sys);

	if (!ok_sys) {
		return 0;
	}

	/*
	 * Coefficient of variation of demand
	 */
	matrix* cov_dem = NULL;
	array* avg_dem = NULL;

	bool ok_dem =
			(ABPN_USE_STATISTICS) ?
					abpnf_getvariation_demand_history(_f, &cov_dem, &avg_dem) :
					abpnf_getvariation_demand_current(_f, &cov_dem, &avg_dem);

	if (!ok_dem) {
		return 0;
	}

	/*
	 * Comparison of system and demand variation
	 */
	double value = abpfn_diffvalue(_f, cov_sys, avg_sys, cov_dem, avg_dem);

	/*
	 * cleanup
	 */
	matrix_destroy(cov_sys);
	array_destroy(avg_sys);
	matrix_destroy(cov_dem);
	array_destroy(avg_dem);

	if (isprint) {
		printf("value=%f \n", value);
		printf("+++++ End abpfn_getsystemvalue(): +++++\n");
	}

	return value;
}
