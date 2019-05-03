package main

import (
	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"

	safe "kube-safe-scheduler/safe"
)

/**
 * Define safe overload predicate
 */

// SafeOverloadPredicate : safe node overload predicate
var SafeOverloadPredicate = Predicate{
	Name: safe.PredicateSafeOverloadName,
	Func: func(pod v1.Pod, node v1.Node) (bool, error) {
		return safe.PredicateFunc(pod, node)
	},
}

/**
 * Define safe priority functions
 */

// SafeBalancePriority : safe balancing across cluster by
// attempting to equalize (mu + sigma) of all nodes,
// i.e. spread onto node with least value of (mu + sigma)
var SafeBalancePriority = Prioritize{
	Name: safe.PrioritySafeBalanceName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return safe.PriorityFunc(pod, nodes, true)
	},
}

// SafeOverloadPriority : safe node overload priority function
var SafeOverloadPriority = Prioritize{
	Name: safe.PrioritySafeOverloadName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return safe.PriorityFunc(pod, nodes, false)
	},
}
