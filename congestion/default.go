package congestion

/**
 * Names of predicates and priority functions
 */
const (
	// PriorityCongestionName : name of  congestion priority function
	PriorityCongestionName = "congestion"
)

/**
 * Key labels
 */
const (
	// CongestionFactorKey : label for measure of node congestion [0 (no congesion) - max]
	CongestionFactorKey = "congestion-factor"
)

/**
 * Environment variables and their default values
 */
const (
	// CongestionFactorMax : environment variable to set maximum congestion factor
	CongestionFactorMax = "CONGESTION_FACTOR_MAX"

	// DefaultCongestionFactorMax : 100
	DefaultCongestionFactorMax = 100
)
