package main

import (
	"k8s.io/apimachinery/pkg/types"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

// Bind :
type Bind struct {
	Func func(podName string, podNamespace string, podUID types.UID, node string) error
}

// Handler :
func (b Bind) Handler(args schedulerapi.ExtenderBindingArgs) *schedulerapi.ExtenderBindingResult {
	err := b.Func(args.PodName, args.PodNamespace, args.PodUID, args.Node)
	return &schedulerapi.ExtenderBindingResult{
		Error: err.Error(),
	}
}
