package main

import (
	"log"
	"os"
	"strconv"

	"k8s.io/client-go/util/integer"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

/**
 * Default names and values
 */
const (
	// PriorityCongestionName : name of  congestion priority function
	PriorityCongestionName = "congestion"

	// CongestionFactorKey : label for measure of node congestion [0 (no congesion) - max]
	CongestionFactorKey = "congestion-factor"

	// CongestionFactorMax : environment variable to set maximum congestion factor
	CongestionFactorMax = "CONGESTION_FACTOR_MAX"

	// DefaultCongestionFactorMax : 100
	DefaultCongestionFactorMax = 100
)

/**
 * Define congestion priority function
 */
var (
	// CongestionPriority : priority function based on node congestion factor
	CongestionPriority = Prioritize{
		Name: PriorityCongestionName,
		Func: func(pod v1.Pod, nodes []v1.Node) (*schedulerapi.HostPriorityList, error) {

			var priorityList schedulerapi.HostPriorityList
			priorityList = make([]schedulerapi.HostPriority, len(nodes))

			var max = DefaultCongestionFactorMax
			if maxValue := os.Getenv(CongestionFactorMax); len(maxValue) > 0 {
				mv, err := strconv.Atoi(maxValue)
				if err == nil && mv > 0 {
					max = mv
				}
			}

			log.Printf("==> CongestionPriority: calculating priority for pod %s based on node congestion in range [0-%d] \n",
				pod.Name, max)

			for i, node := range nodes {
				var cf float64
				cfValue := node.Annotations[CongestionFactorKey]
				if len(cfValue) > 0 {
					v, err := strconv.Atoi(cfValue)
					if err == nil && v > 0 {
						cf = float64(integer.IntMin(v, max))
					}
				}

				score := integer.RoundToInt32(schedulerapi.MaxPriority * (1 - cf/float64(max)))
				log.Printf("node=%s; cf=%f; score=%d \n", node.Name, cf, score)

				priorityList[i] = schedulerapi.HostPriority{
					Host:  node.Name,
					Score: int(score),
				}
			}
			return &priorityList, nil
		},
	}
)
