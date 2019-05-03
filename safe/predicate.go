package safe

import (
	"bytes"
	"fmt"
	"log"

	v1 "k8s.io/api/core/v1"
	schedulercache "k8s.io/kubernetes/pkg/scheduler/cache"
)

// PredicateFunc : safe predicate function
func PredicateFunc(pod v1.Pod, node v1.Node) (bool, error) {

	log.Printf("==> SafeOverloadPredicate: applying SafePredicate for pod %s on node %s ... \n",
		pod.Name, node.Name)

	var buf bytes.Buffer

	podRequest := getResourceRequest(&pod)
	okCPU, outCPU, _ := CPUSafePredicate(&node, podRequest)
	fmt.Fprintf(&buf, "%s", outCPU)

	okMemory, outMemory, _ := MemorySafePredicate(&node, podRequest)
	fmt.Fprintf(&buf, "%s", outMemory)

	okOverall := okCPU && okMemory

	fmt.Fprintf(&buf, "okCPU = %v; okMemory = %v; okOverall = %v; \n", okCPU, okMemory, okOverall)
	log.Print(buf.String())

	return okOverall, nil
}

// MemorySafePredicate : compute safe predicate for memory
func MemorySafePredicate(node *v1.Node, podRequest *schedulercache.Resource) (bool, string, error) {

	var buf bytes.Buffer
	fmt.Fprintf(&buf, "Checking memory fit based on usage history : \n")
	rs, isValid := getMemoryStats(node, podRequest, &buf)
	if !isValid {
		return true, buf.String(), nil
	}

	risk, out := computeRisk(rs)
	fmt.Fprintf(&buf, "%s", out)

	utilizationEpsilon := getSafePercentile()
	if risk > utilizationEpsilon {
		fmt.Fprintf(&buf, "(> %f) rejected! \n", utilizationEpsilon)
		return false, buf.String(), nil
	}
	fmt.Fprintf(&buf, "(<= %f) accepted! \n", utilizationEpsilon)
	return true, buf.String(), nil
}

// CPUSafePredicate : compute safe predicate for CPU
func CPUSafePredicate(node *v1.Node, podRequest *schedulercache.Resource) (bool, string, error) {

	var buf bytes.Buffer
	fmt.Fprintf(&buf, "Checking cpu fit based on usage history : \n")
	rs, isValid := getCPUStats(node, podRequest, &buf)
	if !isValid {
		return true, buf.String(), nil
	}

	risk, out := computeRisk(rs)
	fmt.Fprintf(&buf, "%s", out)

	utilizationEpsilon := getSafePercentile()
	if risk > utilizationEpsilon {
		fmt.Fprintf(&buf, "(> %f) rejected! \n", utilizationEpsilon)
		return false, buf.String(), nil
	}
	fmt.Fprintf(&buf, "(<= %f) accepted! \n", utilizationEpsilon)
	return true, buf.String(), nil
}

// computeRisk : compute risk given usage statistics
func computeRisk(rs *resourceStats) (float64, string) {

	/**
	 * compute node rank given mu and sigma of utilization
	 */
	mu, sigma := getMuSigma(rs)
	utilizationThreshold := getSafeUtilization()
	rnk, out := computeRank(mu, sigma, utilizationThreshold)
	risk := 1. - rnk

	var buf bytes.Buffer
	fmt.Fprintf(&buf, "%s", out)
	fmt.Fprintf(&buf, "mu = %f; sigma = %f; risk(%f) = %f \t", mu, sigma, utilizationThreshold, risk)
	return risk, buf.String()
}
