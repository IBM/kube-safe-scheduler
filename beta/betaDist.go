package beta

import (
	"bytes"
	"fmt"
	"math"
)

// BetaDistribution :
//  http://en.wikipedia.org/wiki/Beta_distribution
//  alpha shape parameter (alpha > 0)
//  beta shape parameter (beta > 0)
type BetaDistribution struct {
	alpha        float64
	beta         float64
	isValid      bool
	firstMoment  float64
	secondMoment float64
	thirdMoment  float64
}

// NewBetaDistribution : constructor
func NewBetaDistribution(alpha, beta float64) *BetaDistribution {
	b := new(BetaDistribution)
	b.isValid = checkValidity(alpha, beta)
	if !b.isValid {
		return nil
	}

	b.alpha = alpha
	b.beta = beta

	b.computeMoments()
	return b
}

// checkValidity : check validity of parameters
func checkValidity(alpha, beta float64) bool {
	return alpha > 0 && beta > 0
}

// computeMoments : compute the moments of the distribution
func (b *BetaDistribution) computeMoments() bool {
	if !b.isValid {
		return false
	}
	b.firstMoment = b.alpha / (b.alpha + b.beta)
	b.secondMoment = b.firstMoment * (b.alpha + 1) / (b.alpha + b.beta + 1)
	b.thirdMoment = b.secondMoment * (b.alpha + 2) / (b.alpha + b.beta + 2)
	return true
}

// Mean : E[X], the mean of the random variable X.
func (b *BetaDistribution) Mean() float64 {
	return b.firstMoment
}

// Variance :
// V[X], the variance of the random variable X.
// V[X] = E[X^2] - (E[X])^2
func (b *BetaDistribution) Variance() float64 {
	return b.secondMoment - b.firstMoment*b.firstMoment
}

// DistributionFunction :
// Probability distribution function, PDF(x) of random variable X.
// The probability that X <= x.
func (b *BetaDistribution) DistributionFunction(x float64) float64 {
	return RegularizedIncomplete(x, b.alpha, b.beta)
}

// DensityFunction :
// Probability density function, pdf(x) of random variable X.
// The probability that x < X < x + dx.
func (b *BetaDistribution) DensityFunction(x float64) float64 {
	var betax = math.Pow(x, b.alpha-1.) * math.Pow((1.-x), (b.beta-1.))
	var betac = Complete(b.alpha, b.beta)
	var pdf = betax / betac
	if math.IsNaN(pdf) || pdf < 0 {
		pdf = 0
	}
	return pdf
}

// MeanResidualLife :
// E[X | age], expected residual life of X given a job age.
// "Representing the Mean Residual Life in Terms of the Failure Rate"
// by R. C. Gupta and D. M. Bradley
func (b *BetaDistribution) MeanResidualLife(age float64) float64 {
	var survival = 1 - b.DistributionFunction(age)
	if survival == 0 {
		return 0
	}
	var failureRate = b.DensityFunction(age) / survival
	if math.IsInf(failureRate, 1) {
		return failureRate
	}
	var m = failureRate * (age * (1 - age)) / (b.alpha + b.beta)
	m += b.firstMoment - age
	return m
}

// MatchMoments : Match the first two moments: m1 and m2
func (b *BetaDistribution) MatchMoments(m1, m2 float64) bool {
	variance := m2 - m1*m1
	if m1 < 0 || m1 > 1 || variance < 0 || variance >= m1*(1-m1) {
		return false
	}
	temp := (m1 * (1 - m1) / variance) - 1
	temp = math.Max(temp, math.SmallestNonzeroFloat64)
	b.alpha = m1 * temp
	b.beta = (1 - m1) * temp
	return b.computeMoments()
}

// GetAlpha : the alpha parameter
func (b *BetaDistribution) GetAlpha() float64 {
	return b.alpha
}

// GetBeta : the beta parameter
func (b *BetaDistribution) GetBeta() float64 {
	return b.beta
}

// PrintTable : print the percentile table
func (b *BetaDistribution) PrintTable() string {
	var buf bytes.Buffer
	fmt.Fprintf(&buf, "x \t pdf(x) \t PDF(x) \t m(x) \n")

	var nMax = 20
	var delta = 1. / float64(nMax)
	for n := 0; n <= nMax; n++ {
		x := float64(n) * delta
		x = math.Min(x, 1)
		fmt.Fprintf(&buf, "%f \t %f \t %f \t %f \n", x, b.DensityFunction(x), b.DistributionFunction(x), b.MeanResidualLife(x))
	}
	return buf.String()
}

// Print : toString() of the distribution
func (b *BetaDistribution) Print() string {
	var buf bytes.Buffer
	fmt.Fprintf(&buf, "BetaDistribution: ")
	fmt.Fprintf(&buf, "alpha = %f; beta = %f; ", b.alpha, b.beta)
	fmt.Fprintf(&buf, "mean = %f; var = %f; ", b.Mean(), b.Variance())
	fmt.Fprintf(&buf, "m1 = %f; m2 = %f; m3 = %f; ", b.firstMoment, b.secondMoment, b.thirdMoment)
	fmt.Fprintf(&buf, "\n")
	return buf.String()
}
