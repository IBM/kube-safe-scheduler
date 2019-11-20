// +build safe all

package main

import (
	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"

	safe "kube-safe-scheduler/safe"
)

// Initialize agent
func init() {
	InitMain()

	// add agent predicates
	AddRouterPredicate(safeOverloadPredicate)

	// add agent priority functions
	AddRouterPriority(safeBalancePriority)
	AddRouterPriority(safeOverloadPriority)
}

/**
 * Define safe overload predicate
 */

// safeOverloadPredicate : safe node overload predicate
var safeOverloadPredicate = Predicate{
	Name: safe.PredicateSafeOverloadName,
	Func: func(pod v1.Pod, node v1.Node) (bool, error) {
		return safe.PredicateFunc(pod, node)
	},
}

/**
 * Define safe priority functions
 */

// safeBalancePriority : safe balancing across cluster by
// attempting to equalize (mu + sigma) of all nodes,
// i.e. spread onto node with least value of (mu + sigma)
var safeBalancePriority = Prioritize{
	Name: safe.PrioritySafeBalanceName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return safe.PriorityFunc(pod, nodes, true)
	},
}

// safeOverloadPriority : safe node overload priority function
var safeOverloadPriority = Prioritize{
	Name: safe.PrioritySafeOverloadName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return safe.PriorityFunc(pod, nodes, false)
	},
}
