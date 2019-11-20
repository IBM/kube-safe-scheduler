// +build congestion all

package main

import (
	"kube-safe-scheduler/congestion"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

// Initialize agent
func init() {
	InitMain()

	// add agent priority functions
	AddRouterPriority(congestionPriority)
}

/**
 * Define congestion priority function
 */

// congestionPriority : priority function based on node congestion factor
var congestionPriority = Prioritize{
	Name: congestion.PriorityCongestionName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return congestion.PriorityFunc(pod, nodes)
	},
}
