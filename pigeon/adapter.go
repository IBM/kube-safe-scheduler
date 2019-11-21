package pigeon

import (
	"fmt"
	"strconv"

	v1 "k8s.io/api/core/v1"
)

type resourceRequested struct {
	milliCPU int64
	memory   int64
	pod      int64
	gpu      int64
	storage  int64
}

// getPodResourceRequested : get pod resource request values
func getPodResourceRequested(pod *v1.Pod) resourceRequested {
	result := resourceRequested{}
	for _, container := range pod.Spec.Containers {
		requests := container.Resources.Requests
		result.memory += requests.Memory().Value()
		result.milliCPU += requests.Cpu().MilliValue()
		//		result.gpu += requests.NvidiaGPU().Value()
		reqGPU := requests["gpu"]
		requestedGPU, ok := reqGPU.AsInt64()
		if !ok {
			result.gpu += requestedGPU
		}
		result.storage += requests.StorageEphemeral().Value()
	}
	result.pod = 1
	return result
}

// MakeNodeTemplate : make a node info template for a node
func MakeNodeTemplate(node *v1.Node, numResources int) *Node {

	nodeTemplate := new(Node)
	nodeTemplate.ID = node.Name

	nodeTemplate.ResourceCapacity = make([]int64, numResources)
	nodeTemplate.ResourceOverflow = make([]bool, numResources)
	nodeTemplate.ResourceUsage = make([]int64, numResources)

	allocatableResourceMap := node.Status.Allocatable

	// CPU
	totalMilliCPU := allocatableResourceMap.Cpu().MilliValue()
	nodeTemplate.ResourceCapacity[0] = totalMilliCPU
	nodeTemplate.ResourceOverflow[0] = false
	usedCPUStr := node.ObjectMeta.Annotations[RequestedCPUKey]
	nodeTemplate.ResourceUsage[0], _ = strconv.ParseInt(usedCPUStr, 10, 64)

	// Memory
	if numResources > 1 {
		totalMemory := allocatableResourceMap.Memory().Value()
		// make memory units in GB to save 20 bits
		nodeTemplate.ResourceCapacity[1] = totalMemory / 1024 / 1024
		nodeTemplate.ResourceOverflow[1] = false
		usedMemoryStr := node.ObjectMeta.Annotations[RequestedMemoryKey]
		usedMemory, _ := strconv.ParseInt(usedMemoryStr, 10, 64)
		nodeTemplate.ResourceUsage[1] = usedMemory / 1024 / 1024
	}

	// Pods
	if numResources > 2 {
		totalPods := allocatableResourceMap.Pods().Value()
		nodeTemplate.ResourceCapacity[2] = totalPods
		nodeTemplate.ResourceOverflow[2] = false
		usedPodStr := node.ObjectMeta.Annotations[RequestedPodKey]
		nodeTemplate.ResourceUsage[2], _ = strconv.ParseInt(usedPodStr, 10, 64)
	}

	// GPU
	if numResources > 3 {
		//		totalGPU := allocatableResourceMap.NvidiaGPU().Value()
		quantityGPU := allocatableResourceMap["gpu"]
		totalGPU, ok := quantityGPU.AsInt64()
		if !ok {
			totalGPU = 0
		}
		nodeTemplate.ResourceCapacity[3] = totalGPU
		nodeTemplate.ResourceOverflow[3] = false
		usedGPUStr := node.ObjectMeta.Annotations[RequestedGPUKey]
		nodeTemplate.ResourceUsage[3], _ = strconv.ParseInt(usedGPUStr, 10, 64)
	}

	// ephemeral-storage
	if numResources > 4 {
		totalStorage := allocatableResourceMap.StorageEphemeral().Value()
		// make storage units in TB
		nodeTemplate.ResourceCapacity[4] = totalStorage / 1024 / 1024 / 1024
		nodeTemplate.ResourceOverflow[4] = false
		usedStorageStr := node.ObjectMeta.Annotations[RequestedStorageKey]
		requestedStorage, _ := strconv.ParseInt(usedStorageStr, 10, 64)
		nodeTemplate.ResourceUsage[4] = requestedStorage / 1024 / 1024 / 1024
	}

	return nodeTemplate
}

// CreatePodModel : create a pod request
func CreatePodModel(pod *v1.Pod, numResources int) *PodRequest {

	podTemplate := new(PodRequest)
	podTemplate.ID = string(pod.UID)

	podTemplate.ResourceDemand = make([]int64, numResources)

	podResource := getPodResourceRequested(pod)
	demandCPU := podResource.milliCPU
	demandMemory := podResource.memory
	demandPod := podResource.pod
	demandGPU := podResource.gpu
	demandStorage := podResource.storage

	fmt.Printf("===> demandCPU=%d, demandMemory=%d, demandPod=%d, demandGPU=%d, demandStorage=%d \n",
		demandCPU, demandMemory, demandPod, demandGPU, demandStorage)
	// TODO: make minimum resource requirements configurable
	if demandCPU == 0 {
		demandCPU = 10
	}
	demandMemory = demandMemory / 1024 / 1024
	if demandMemory == 0 {
		demandMemory = 16
	}

	fmt.Printf("===> dcpu=%d, dmem=%d, dpod=%d, dgpu=%d, dstg=%d \n",
		demandCPU, demandMemory, demandPod, demandGPU, demandStorage)

	podTemplate.ResourceDemand[0] = demandCPU
	if numResources > 1 {
		podTemplate.ResourceDemand[1] = demandMemory
	}
	if numResources > 2 {
		podTemplate.ResourceDemand[2] = demandPod
	}
	if numResources > 3 {
		podTemplate.ResourceDemand[3] = demandGPU
	}
	if numResources > 4 {
		podTemplate.ResourceDemand[4] = demandStorage
	}

	fmt.Printf("podRequest=%v \n", *podTemplate)
	return podTemplate
}
