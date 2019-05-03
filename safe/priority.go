package safe

import (
	"bytes"
	"fmt"
	"log"
	"math"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
	schedulercache "k8s.io/kubernetes/pkg/scheduler/cache"
)

// PriorityFunc : compute safe priority function
func PriorityFunc(pod v1.Pod, nodes []v1.Node, isBalance bool) (*schedulerapi.HostPriorityList, error) {

	var priorityList schedulerapi.HostPriorityList
	priorityList = make([]schedulerapi.HostPriority, len(nodes))

	podRequest := getResourceRequest(&pod)

	for i, node := range nodes {

		log.Printf("==> SafePriority: calculating priority for pod %s on node %s ... \n",
			pod.Name, node.Name)

		var buf bytes.Buffer
		fmt.Fprintf(&buf, "Calculating priority based on usage history ... \n")

		CPUScore, okCPU, outCPU := cpuScore(&node, podRequest, isBalance)
		memoryScore, okMemory, outMemory := memoryScore(&node, podRequest, isBalance)

		fmt.Fprintf(&buf, "%s", outCPU)
		fmt.Fprintf(&buf, "%s", outMemory)

		var score int
		if okMemory {
			if okCPU {
				score = memoryScore
				if CPUScore < memoryScore {
					score = CPUScore
				}
			} else {
				score = memoryScore
			}
		} else {
			if okCPU {
				score = CPUScore
			}
		}

		fmt.Fprintf(&buf, "overallScore = %d \n", score)
		log.Print(buf.String())

		priorityList[i] = schedulerapi.HostPriority{
			Host:  node.Name,
			Score: int(score),
		}
	}
	return &priorityList, nil
}

// memoryScore : compute score for memory based on usage history
func memoryScore(node *v1.Node, podRequest *schedulercache.Resource, isBalance bool) (int, bool, string) {

	var buf bytes.Buffer
	fmt.Fprintf(&buf, "Memory usage statistics: \n")
	rs, isValid := getMemoryStats(node, podRequest, &buf)
	if !isValid {
		return 0, false, buf.String()
	}

	score, out := computeScore(rs, isBalance)
	fmt.Fprintf(&buf, "%s", out)
	fmt.Fprintf(&buf, "memoryScore = %d \n", score)
	return score, true, buf.String()
}

// cpuScore : compute score for CPU based on usage history
func cpuScore(node *v1.Node, podRequest *schedulercache.Resource, isBalance bool) (int, bool, string) {

	var buf bytes.Buffer
	fmt.Fprintf(&buf, "CPU usage statistics: \n")
	rs, isValid := getCPUStats(node, podRequest, &buf)
	if !isValid {
		return 0, false, buf.String()
	}

	score, out := computeScore(rs, isBalance)
	fmt.Fprintf(&buf, "%s", out)
	fmt.Fprintf(&buf, "CPUScore = %d \n", score)
	return score, true, buf.String()
}

// computeScore : compute score given usage statistics
func computeScore(rs *resourceStats, isBalance bool) (int, string) {

	var buf bytes.Buffer
	mu, sigma := getMuSigma(rs)

	if isBalance {
		fmt.Fprintf(&buf, "SafeBalancing: ")
		obj := (mu + math.Sqrt(sigma)) / 2
		fmt.Fprintf(&buf, "mu = %f; sigma = %f; obj = %f \n", mu, sigma, obj)
		objScaled := (1. - obj) * schedulerapi.MaxPriority
		score := int(objScaled + 0.5)
		return score, buf.String()
	}

	/**
	 * compute node rank given mu and sigma of utilization
	 */
	utilizationThreshold := getSafeUtilization()
	utilizationEpsilon := getSafePercentile()
	rnk, out := computeRank(mu, sigma, utilizationThreshold)
	fmt.Fprintf(&buf, "%s", out)
	var risk = 1. - rnk
	var riskFraction = risk / utilizationEpsilon
	riskFraction = math.Min(riskFraction, 1.)

	fmt.Fprintf(&buf, "mu = %f; sigma = %f; risk(%f) = %f; riskFraction = %f \n", mu, sigma, utilizationThreshold, risk, riskFraction)

	score := int(0)
	if riskFraction >= 1 {
		score = 0
	} else {
		score = int((1. - math.Sqrt(riskFraction)) * schedulerapi.MaxPriority)
	}
	return score, buf.String()
}
