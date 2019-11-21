package pigeon

import (
	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

// PriorityFunc : compute pigeon priority function
func PriorityFunc(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {
	var priorityList schedulerapi.HostPriorityList
	priorityList = make([]schedulerapi.HostPriority, len(nodes))

	agent := PigeonAgent
	if agent != nil {
		agent.UpdateState(&nodes)
		rankMap, err := agent.ComputeRank(&pod)
		if err == nil {
			var i int = 0
			for k, v := range *rankMap {
				priorityList[i] = schedulerapi.HostPriority{
					Host:  k,
					Score: v,
				}
				i++
			}
		}
	} else {
		for i := 0; i < len(nodes); i++ {
			priorityList[i] = schedulerapi.HostPriority{
				Host:  nodes[i].GetName(),
				Score: 0,
			}
		}
	}
	return &priorityList, nil
}
