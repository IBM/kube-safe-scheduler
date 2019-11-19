package foo

import (
	"log"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

// PriorityFunc : priority function
func PriorityFunc(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {

	log.Printf("==> FooPriority: calculating priorities for pod %s ... \n", pod.Name)

	var priorityList schedulerapi.HostPriorityList
	priorityList = make([]schedulerapi.HostPriority, len(nodes))

	for i, node := range nodes {
		priorityList[i] = schedulerapi.HostPriority{
			Host:  node.Name,
			Score: int(1),
		}
	}
	return &priorityList, nil
}
