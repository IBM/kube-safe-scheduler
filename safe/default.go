package safe

/**
 * Names of predicates and priority functions
 */
const (
	// PredicateSafeOverloadName : name of safe overload predicate
	PredicateSafeOverloadName = "safe-overload"
	// PrioritySafeOverloadName : name of safe overload priority function
	PrioritySafeOverloadName = "safe-overload"
	// PrioritySafeBalanceName : name of safe balance priority function
	PrioritySafeBalanceName = "safe-balance"
)

/**
 * Key labels
 */
const (
	// MeanFreeMemoryKey : key for the mean free memory
	MeanFreeMemoryKey = "mean-free-memory"
	// StdFreeMemoryKey : key for the standard deviation free memory
	StdFreeMemoryKey = "std-free-memory"
	// MeanFreeCPUKey : key for the mean free CPU
	MeanFreeCPUKey = "mean-free-cpu"
	// StdFreeCPUKey : key for the standard deviation free CPU
	StdFreeCPUKey = "std-free-cpu"
	// ForcatedFreeMemoryKey : key for the forecasted free memory
	ForcastedFreeMemoryKey = "forcasted-free-memory"
	// ForcatedFreeCPUKey : key for the forecasted free CPU
	ForcastedFreeCPUKey = "forcasted-free-cpu"
)

/**
 * Environment variables and their default values
 */
const (
	// SafeUtilization : safe value target utilization
	SafeUtilization = "SAFEUTILIZATION"

	// SafePercentile : safe value of acceptable percentile above target
	SafePercentile = "SAFEPERCENTILE"

	// SafePrintTable : safe option to print table
	SafePrintTable = "SAFEPRINTTABLE"

	// SafeForecastWeight : safe weight to include forecasted figures
	SafeForecastWeight = "SAFEFORECASTWEIGHT"

	// DefaultSafeUtilization : default value for the safe utilization parameter
	DefaultSafeUtilization = 0.90

	// DefaultSafePercentile : default value for the safe percentile parameter
	DefaultSafePercentile = 0.30

	// DefaultForecastWeight : default value for the weight of forcasted value
	DefaultForecastWeight = 0.20
)
