package safe

import (
	"bytes"
	"fmt"
	"kube-safe-scheduler/beta"
	"math"
	"os"
	"strconv"

	v1 "k8s.io/api/core/v1"
	schedulercache "k8s.io/kubernetes/pkg/scheduler/cache"
)

// getResourceRequest : get the resource demand of a pod (CPU and Memory only)
func getResourceRequest(pod *v1.Pod) *schedulercache.Resource {

	result := &schedulercache.Resource{}
	for _, container := range pod.Spec.Containers {
		result.Add(container.Resources.Requests)
	}

	// take max_resource(sum_pod, any_init_container)
	for _, container := range pod.Spec.InitContainers {
		for rName, rQuantity := range container.Resources.Requests {
			switch rName {
			case v1.ResourceCPU:
				if CPU := rQuantity.MilliValue(); CPU > result.MilliCPU {
					result.MilliCPU = CPU
				}
			case v1.ResourceMemory:
				if mem := rQuantity.Value(); mem > result.Memory {
					result.Memory = mem
				}
			default:
			}
		}
	}
	return result
}

// computeRank : Rank (or utility) is defined as the probability that the utilization of
// the saturated resource is less than or equal to a given utilization limit.
func computeRank(mu, sigma, threshold float64) (float64, string) {

	var buf bytes.Buffer
	if mu == 0 || (sigma == 0 && mu <= threshold) {
		return 1, buf.String()
	}

	if sigma == 0 && mu > threshold {
		return 0, buf.String()
	}

	m1 := mu
	m2 := (sigma * sigma) + (mu * mu)

	betaDist := beta.NewBetaDistribution(1, 1)
	if !betaDist.MatchMoments(m1, m2) {
		return 0, buf.String()
	}
	belowLimit := betaDist.DistributionFunction(threshold)

	fmt.Fprintf(&buf, "%s", betaDist.Print())
	if safePrintTableValue := os.Getenv(SafePrintTable); safePrintTableValue == "true" {
		fmt.Fprintf(&buf, "%s", betaDist.PrintTable())
	}

	if math.IsNaN(belowLimit) {
		return 1, buf.String()
	}
	return belowLimit, buf.String()
}

// getSafeUtilization : the safe utilization value
func getSafeUtilization() float64 {

	utilizationThreshold := DefaultSafeUtilization
	if safeUtilizationValue := os.Getenv(SafeUtilization); len(safeUtilizationValue) > 0 {
		suv, err := strconv.Atoi(safeUtilizationValue)
		if err == nil && suv >= 0 && suv <= 100 {
			utilizationThreshold = float64(suv) / float64(100)
		}
	}
	return utilizationThreshold
}

// getSafePercentile : the safe percentile value
func getSafePercentile() float64 {

	utilizationEpsilon := DefaultSafePercentile
	if safePercentileValue := os.Getenv(SafePercentile); len(safePercentileValue) > 0 {
		spv, err := strconv.Atoi(safePercentileValue)
		if err == nil && spv >= 0 && spv <= 100 {
			utilizationEpsilon = float64(spv) / float64(100)
		}
	}
	return utilizationEpsilon
}

// getForecastWeight: the weighht for forecasting
func getForecastWeight() float64 {

	forecastWeight := DefaultForecastWeight
	if forecastWeightValue := os.Getenv(SafeForecastWeight); len(forecastWeightValue) > 0 {
		fwv, err := strconv.Atoi(forecastWeightValue)
		if err == nil && fwv >= 0 && fwv <= 100 {
			forecastWeight = float64(fwv) / float64(100)
		}
	}
	return forecastWeight
}

// resourceStats : statistics for a resource
type resourceStats struct {
	// average used
	usedAvg float64
	// standard deviation used
	usedStdev float64
	// demand of pod
	demand float64
	// node capacity
	capacity float64
}

// getMuSigma : get average and standard deviation from statistics
func getMuSigma(rs *resourceStats) (float64, float64) {

	mu := (rs.usedAvg + rs.demand) / rs.capacity
	mu = math.Max(math.Min(mu, 1), 0)
	sigma := rs.usedStdev / rs.capacity
	sigma = math.Max(math.Min(sigma, 1), 0)
	return mu, sigma
}

// getMemoryStats : get statistics for memory resource
func getMemoryStats(node *v1.Node,

	podRequest *schedulercache.Resource,
	buf *bytes.Buffer) (rs *resourceStats, isValid bool) {

	meanFreeMemory := node.ObjectMeta.Annotations[MeanFreeMemoryKey]
	freeAvg, err := strconv.ParseFloat(meanFreeMemory, 64)
	if err != nil {
		fmt.Fprintf(buf, "No valid memory statistics. \n")
		return nil, false
	}

	rs = &resourceStats{}
	stdFreeMemory := node.ObjectMeta.Annotations[StdFreeMemoryKey]
	freeStdev, err := strconv.ParseFloat(stdFreeMemory, 64)
	if err != nil {
		freeStdev = 0
	}
	fmt.Fprintf(buf, "meanFreeMemory = %s; stdFreeMemory = %s \n", meanFreeMemory, stdFreeMemory)

	safeForecastWeight := getForecastWeight()
	meanFreeForecastMemory := node.ObjectMeta.Annotations[ForcastedFreeMemoryKey]
	freeForecastAvg, err := strconv.ParseFloat(meanFreeForecastMemory, 64)
	if err == nil && len(meanFreeForecastMemory) > 0 {
		freeAvg = (1.-safeForecastWeight)*freeAvg + safeForecastWeight*freeForecastAvg
		fmt.Fprintf(buf, "forcastedFreeMemory = %s; safeForecastWeight = %f; weightedFreeMemory = %f \n",
			meanFreeForecastMemory, safeForecastWeight, freeAvg)
	}

	var megaFactor = float64(1. / 1024. / 1024.)
	freeAvg *= megaFactor
	freeStdev *= megaFactor
	fmt.Fprintf(buf, "freeAvg = %f MB; freeStdev = %f MB \n", freeAvg, freeStdev)

	allocatableResources := node.Status.Allocatable
	if am := allocatableResources["memory"]; &am != nil {
		rs.capacity = float64((&am).Value())
	}
	rs.capacity *= megaFactor
	rs.demand = float64(podRequest.Memory) * megaFactor
	fmt.Fprintf(buf, "capacity = %f MB; demand = %f MB \n", rs.capacity, rs.demand)

	rs.usedAvg = math.Max(rs.capacity-freeAvg, 0)
	rs.usedStdev = freeStdev
	fmt.Fprintf(buf, "usedAvg = %f; usedStdev = %f \n", rs.usedAvg, rs.usedStdev)
	return rs, true
}

// getCPUStats : get statistics for CPU resource
func getCPUStats(node *v1.Node,

	podRequest *schedulercache.Resource,
	buf *bytes.Buffer) (rs *resourceStats, isValid bool) {

	meanFreeCPU := node.ObjectMeta.Annotations[MeanFreeCPUKey]
	freeAvg, err := strconv.ParseFloat(meanFreeCPU, 64)
	if err != nil {
		fmt.Fprintf(buf, "No valid CPU statistics. \n")
		return nil, false
	}

	rs = &resourceStats{}
	stdFreeCPU := node.ObjectMeta.Annotations[StdFreeCPUKey]
	freeStdev, err := strconv.ParseFloat(stdFreeCPU, 64)
	if err != nil {
		freeStdev = 0
	}
	fmt.Fprintf(buf, "meanFreeCPU = %s; stdFreeCPU = %s \n", meanFreeCPU, stdFreeCPU)

	safeForecastWeight := getForecastWeight()
	meanFreeForecastCPU := node.ObjectMeta.Annotations[ForcastedFreeCPUKey]
	freeForecastAvg, err := strconv.ParseFloat(meanFreeForecastCPU, 64)
	if err == nil && len(meanFreeForecastCPU) > 0 {
		freeAvg = (1.-safeForecastWeight)*freeAvg + safeForecastWeight*freeForecastAvg
		fmt.Fprintf(buf, "forcastedFreeCPU = %s; safeForecastWeight = %f; weightedFreeCPU = %f \n",
			meanFreeForecastCPU, safeForecastWeight, freeAvg)
	}

	fmt.Fprintf(buf, "freeAvg = %f; freeStdev = %f \n", freeAvg, freeStdev)

	allocatableResources := node.Status.Allocatable
	if am := allocatableResources["cpu"]; &am != nil {
		rs.capacity = float64((&am).MilliValue())
	}
	rs.demand = float64(podRequest.MilliCPU)
	fmt.Fprintf(buf, "capacity = %f; demand = %f \n", rs.capacity, rs.demand)

	rs.usedAvg = math.Max(rs.capacity-freeAvg, 0)
	rs.usedStdev = freeStdev
	fmt.Fprintf(buf, "usedAvg = %f; usedStdev = %f \n", rs.usedAvg, rs.usedStdev)
	return rs, true
}
