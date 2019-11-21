// +build pigeon all

package main

import (
	pigeon "kube-safe-scheduler/pigeon"
	"log"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

// Initialize agent
func init() {
	InitMain()

	// create a pigeon client and register it (for persistence)
	log.Println("info: creating pigeon instance ...")
	pigeon.PigeonAgent = pigeon.NewAgent()

	// add agent priority functions
	AddRouterPriority(PigeonPriority)
}

/**
 * Define pigeon priority functions
 */

// PigeonPriority : pigeon priority function
var PigeonPriority = Prioritize{
	Name: pigeon.PriorityPigeonName,
	Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
		return pigeon.PriorityFunc(pod, nodes)
	},
}
