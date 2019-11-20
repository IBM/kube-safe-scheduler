package foo

import (
	"log"

	v1 "k8s.io/api/core/v1"
)

// PredicateFunc : predicate function
func PredicateFunc(pod v1.Pod, node v1.Node) (bool, error) {

	log.Printf("==> FooPredicate: applying predicate for pod %s on node %s ... \n",
		pod.Name, node.Name)

	return true, nil
}
