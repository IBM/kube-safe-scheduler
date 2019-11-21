package pigeon

/**
 * Names of predicates and priority functions
 */
const (
	// PriorityPigeonName : name of pigeon priority function
	PriorityPigeonName = "pigeon-holing"
)

/**
 * Key labels
 */
const (
	// RequestedCPUKey : key for the requested CPU
	RequestedCPUKey = "requested-cpu"
	// RequestedMemoryKey : key for the requested memory
	RequestedMemoryKey = "requested-memory"
	// RequestedPodKey : key for the requested pods
	RequestedPodKey = "requested-pods"
	// RequestedGPUKey : key for the requested GPU
	RequestedGPUKey = "requested-gpu"
	// RequestedStorageKey : key for the requested storage
	RequestedStorageKey = "requested-ephemeral-storage"
)

/**
 * Environment variables and their default values
 */
const (
	// DefaultNumResources :
	DefaultNumResources = 2
)
