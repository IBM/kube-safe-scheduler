/*
 * commonobj.c
 *
 *  Created on: Oct 17, 2018
 *      Author: tantawi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "commonobj.h"

#include "../utils/mathhelper.h"


double commonobj_balance(float* x, int n) {
	double val = mathhelper_stddev(x, n);
	return val;
}

double unbalance_basic(float* x, int n) {
	double val = 0;
	for (int i = 0; i < n; ++i) {
		if (x[i] > 0 && x[i] < 1) {
			val += sqrt(x[i]);
		}
	}
	val /= n;
	return val;
}

double unbalance_stdev(float* x, int n) {
	double val = mathhelper_stddev(x, n);
	val = fmin(val, 1);
	return 1 - val;
}

double unbalance_scaled(float* x, int n) {
	double avg = mathhelper_average(x, n);
	if (avg <= 0 || avg >= 1) {
		return 0;
	}
	double sum = 0;
	for (int i = 0; i < n; ++i) {
		if (x[i] <= 0 && x[i] >= 1) {
			sum += 1;
		} else {
			double term = 0;
			if (x[i] < avg) {
				term = (avg - x[i]) / avg;
			} else if (x[i] > avg) {
				term = (x[i] - avg) / (1 - avg);
			}
			sum += (term * term);
		}
	}
	double val = sum / n;
	val = sqrt(val);
	val = 1 - val;
	return val;
}

double unbalance_mixed(float* x, int n) {
	int n0 = 0;			// number of empty bins
	double product = 1;
	double xMax = 0;

	for (int i = 0; i < n; ++i) {
		if (x[i] == 0) {
			n0++;
		} else {
			double x1 = fmin(x[i], 1);
			product *= x1;
			if (x1 > xMax) {
				xMax = x1;
			}
		}
	}

	if (n0 == n || xMax == 0) {
		return 0;
	}

	// measure of empty bins
	double v1 = (n - n0 - 1) / ((double) n);

	// measure of skewness
	double expn = 1 / ((double) (n - n0));
	double v2 = pow(product, expn);
	v2 /= (xMax * n);

	// combined measure
	//TODO: need to normalize in [0, 1]
	double val = v1 + v2;
	return val;
}

double commonobj_unbalance(float* x, int n, commonobj_unbalance_type type) {
	if (n <= 0 || !x) {
		return 0;
	}

	switch (type) {
	case UNBALANCE_BASIC:
		return unbalance_basic(x, n);
		break;
	case UNBALANCE_STDEV:
		return unbalance_stdev(x, n);
		break;
	case UNBALANCE_SCALED:
		return unbalance_scaled(x, n);
		break;
	case UNBALANCE_MIXED:
		return unbalance_mixed(x, n);
		break;
	default:
		return unbalance_basic(x, n);
		break;
	}
}
