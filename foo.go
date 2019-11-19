// +build foo all

package main

import (
	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
	"kube-safe-scheduler/foo"
)

// Initialize agent
func init() {
	InitMain()

	// add agent predicates
	AddRouterPredicate(fooPredicate)

	// add agent priority functions
	AddRouterPriority(fooPriority)
}

/**
 * Define foo predicate
 */

// fooPredicate : predicate
var fooPredicate = Predicate{
	Name: foo.PredicateFooName,
	Func: func(pod v1.Pod, node v1.Node) (bool, error) {
		return foo.PredicateFunc(pod, node)
	},
}

/**
 * Define foo priority function
 */

// fooPriority : priority function
var fooPriority = Prioritize{
	Name: foo.PriorityFooName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return foo.PriorityFunc(pod, nodes)
	},
}
