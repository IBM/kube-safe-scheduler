package beta

import (
	"math"
)

/*
 * Beta related functions
 */

// Complete : complete beta function, B(a,b)
func Complete(a, b float64) float64 {
	return math.Exp(lgamma(a) + lgamma(b) - lgamma(a+b))
}

// Incomplete : B(x;a,b) is the incomplete beta function
func Incomplete(x, a, b float64) float64 {
	return regIncByContFraction(x, a, b, false)
}

// RegularizedIncomplete : I_x(a,b) = B(x;a,b) / B(a,b),
// where B(x;a,b) is the incomplete and B(a,b) is the complete beta function
func RegularizedIncomplete(x, a, b float64) float64 {
	return regIncByContFraction(x, a, b, true)
}

// regIncByContFraction :
// calculate regularized incomplete beta function using continued fraction representation
// see "Numerical Recipes" by Press, Flannery, Teukolsky, and Vetterling,
// Cambridge University Press
// (0 <= x <= 1)
func regIncByContFraction(x, a, b float64, isRegularized bool) float64 {
	var bt float64
	if x == 0 || x == 1 {
		bt = 0
	} else {
		bt = math.Exp((a * math.Log(x)) + (b * math.Log(1-x)))
		if isRegularized && bt != 0 {
			bt /= Complete(a, b)
		}
	}

	var betai float64
	if x < (a+1)/(a+b+2) {
		betai = bt * betaCF(x, a, b) / a
	} else {
		betai = 1 - bt*betaCF(1-x, b, a)/b
	}

	return betai
}

// betaCF : beta continued fraction representation
func betaCF(x, a, b float64) float64 {
	const itmax = 100
	const eps = 3E-7

	var am float64 = 1
	var bm float64 = 1
	var az float64 = 1

	var qab = a + b
	var qap = a + 1
	var qam = a - 1

	var bz = 1 - qab*x/qap

	for m := 1; m <= itmax; m++ {
		mf := float64(m)
		var tm = mf + mf

		// even step
		var d = mf * (b - mf) * x / ((qam + tm) * (a + tm))
		var ap = az + d*am
		var bp = bz + d*bm

		// odd step
		d = -(a + mf) * (qab + mf) * x / ((a + tm) * (qap + tm))
		var app = ap + d*az
		var bpp = bp + d*bz

		// save old answer
		var aold = az

		// renormalize to prevent overflows
		am = ap / bpp
		bm = bp / bpp
		az = app / bpp
		bz = 1

		// check tolerance
		if math.Abs(az-aold) < eps*math.Abs(az) {
			return az
		}
	}

	return az
}

// lgamma : log gamma function
func lgamma(x float64) float64 {
	y, _ := math.Lgamma(x)
	return y
}
