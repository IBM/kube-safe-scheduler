package pigeon

import (
	"fmt"
	"math"

	v1 "k8s.io/api/core/v1"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

var (
	// PigeonAgent :
	PigeonAgent *Agent
)

// Agent :
type Agent struct {
	client *Client
}

// NewAgent creates a pigeon agent
func NewAgent() *Agent {

	configFile := "pigeon.cfg"
	client := NewClient(configFile)

	return &Agent{
		client: client,
	}
}

// UpdateState :
func (a *Agent) UpdateState(nodes *[]v1.Node) bool {
	client := a.client
	if client == nil {
		fmt.Printf("Client missing! \n")
		return false
	}
	numResources := client.GetNumResources()
	isStarted := client.StateUpdateStart()
	if !isStarted {
		fmt.Printf("Failed starting state updater! \n")
		return false
	}

	for _, node := range *nodes {
		myNode := MakeNodeTemplate(&node, numResources)
		ok, err := client.StateUpdateNode(myNode)
		if !ok {
			fmt.Printf("Failed to update node %s: %s \n", node.GetName(), err)
		}
	}

	client.StateUpdateStop()
	return true
}

// ComputeRank :
func (a *Agent) ComputeRank(pod *v1.Pod) (*map[string]int, error) {
	client := a.client
	if client == nil {
		fmt.Printf("Client missing! \n")
		return nil, fmt.Errorf("Client missing")
	}

	fmt.Printf("Evaluating objective for pod %s \n", pod.GetName())
	numResources := client.GetNumResources()
	podRequest := CreatePodModel(pod, numResources)
	valueMap := client.GetPodRanks(podRequest)

	var minValue, maxValue float64
	isSet := false

	for _, v := range valueMap {
		if !math.IsNaN(v) {
			if !isSet {
				minValue = v
				maxValue = v
				isSet = true
			} else {
				if v < minValue {
					minValue = v
				}
				if v > maxValue {
					maxValue = v
				}
			}
		}
	}

	rankMap := make(map[string]int)
	maxPriority := int(schedulerapi.MaxPriority)
	span := maxValue - minValue
	if span < 0 {
		span = 0
	}

	// smaller rank values are better
	for k, v := range valueMap {
		if math.IsNaN(v) {
			rankMap[k] = 0
		} else if span == 0 {
			rankMap[k] = maxPriority
		} else {
			rankMap[k] = int(math.Round(1 + (float64(maxPriority)-1)*(maxValue-v)/span))
			if rankMap[k] < 0 {
				rankMap[k] = 0
			}
			if rankMap[k] > maxPriority {
				rankMap[k] = maxPriority
			}
		}
	}

	fmt.Println("rankMap:", rankMap)
	return &rankMap, nil
}
