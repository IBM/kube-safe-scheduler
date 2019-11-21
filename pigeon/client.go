package pigeon

/*
#include <stdio.h>
#include <stdlib.h>
#include "evaluator.h"
#include "bsagohelper.h"
#include "system/stateupdater.h"
#cgo CFLAGS: -fPIC -std=c99 -I./pigeonmodule/include
#cgo LDFLAGS: -L${SRCDIR}/pigeonmodule/libs -lpigeon -lm
*/
import "C"
import (
	"fmt"
	"os"
	"strconv"
	"unsafe"
)

const (
	version = "0.0.0"

	numResourcesKey     = "NUM_RESOURCES"
	defaultNumResources = 2

	policyObjectiveKey = "POLICY_OBJECTIVE"
)

// Client : client to pigeon
type Client struct {

	// the pigeon evaluator
	eval *C.evaluator

	// the pigeon state updater
	updater *C.stateupdater

	// number of resource types considered
	numResources int

	// model generation number
	genNum int64
}

// Node : node characteristics
type Node struct {
	/* unique ID */
	ID string

	/* resources */
	ResourceCapacity []int64
	ResourceOverflow []bool
	ResourceUsage    []int64
}

// PodRequest : attributes of a pod placement request
type PodRequest struct {
	/* unique ID */
	ID string

	/* resource demand */
	ResourceDemand []int64
}

// PodState : the observed state of a pod
type PodState struct {
	/* pod unique ID */
	ID string

	/* node unique ID */
	NodeID string

	/* resource demand */
	ResourceDemand []int64

	/* running state of pod */
	IsRunning bool
}

// NewClient : create an instance of a pigeon client
func NewClient(configFile string) *Client {
	c := new(Client)
	fmt.Printf("parameters in config file %s: \n", configFile)
	eval := C.evaluator_createfromfile(C.CString(configFile))
	C.fflush(nil)
	if eval != nil {
		fmt.Printf("===> Created pigeon client\n")
		fmt.Printf("Version=%s \n", version)
		c.updater = (*C.stateupdater)(unsafe.Pointer(C.evaluator_getstateupdater(eval)))
	} else {
		fmt.Printf("===> Failed to create pigeon client\n")
		c.updater = nil
	}
	c.eval = eval
	c.genNum = 0

	c.numResources = defaultNumResources
	if numResourcesValue := os.Getenv(numResourcesKey); len(numResourcesValue) > 0 {
		numv, err := strconv.Atoi(numResourcesValue)
		if err == nil && numv > 0 && numv <= 5 {
			fmt.Printf("setting number of resources from environment to %d \n", numv)
			c.numResources = numv
		}
	}
	fmt.Printf("numResources=%d \n", c.numResources)

	if policyObjectiveValue := os.Getenv(policyObjectiveKey); len(policyObjectiveValue) > 0 {
		fmt.Printf("setting policy objective from environment to %s \n", policyObjectiveValue)
		C.evaluator_setpolicyobjective(eval, C.CString(policyObjectiveValue))
	}
	policyObjective := C.GoString(C.evaluator_getpolicyobjective(eval))
	fmt.Printf("policyObjective=%s \n", policyObjective)

	return c
}

// Destroy : destroy the scheduler
func (client *Client) Destroy() {
	C.evaluator_destroy(client.eval)
}

// GetNumResources : get the number of resource types considered
func (client *Client) GetNumResources() int {
	// if client.eval != nil {
	// 	return int(C.evaluator_getnumresources(client.eval))
	// }
	return client.numResources
}

// StateUpdateStart : start an update session for a given generation
func (client *Client) StateUpdateStart() bool {
	client.genNum++
	isStarted := C.stateupdater_start(client.updater, C.long(client.genNum))
	C.fflush(nil)
	if isStarted {
		return true
	}
	return false
}

// StateUpdateStop : stop the update session and print report
func (client *Client) StateUpdateStop() {
	C.stateupdater_stop(client.updater)
	C.stateupdater_print(client.updater)
	C.evaluator_print(client.eval)
	C.fflush(nil)
}

// StateUpdateNode : Update the state of a node
func (client *Client) StateUpdateNode(node *Node) (bool, error) {

	fmt.Printf("===> StateUpdateNode: Updating node %s with [(Capacity,Ovf,Usage)]=[ ", node.ID)
	numResources := len(node.ResourceCapacity)
	if numResources == 0 {
		fmt.Printf("null ] \n")
		return false, fmt.Errorf("StateUpdateNode: failed, no resources specified")
	}

	capacity := C.makeIntArray(C.int(numResources))
	defer C.freeIntArray(capacity)
	ovf := C.makeBoolArray(C.int(numResources))
	defer C.freeBoolArray(ovf)

	updateUsage := (node.ResourceUsage != nil)
	usage := C.makeIntArray(C.int(numResources))
	defer C.freeIntArray(usage)

	for i := 0; i < numResources; i++ {
		ci := C.int(i)
		capi := int32(node.ResourceCapacity[i])
		ovfi := node.ResourceOverflow[i]
		fmt.Printf("(")
		fmt.Printf("%d,%t", capi, ovfi)
		if updateUsage {
			usei := int32(node.ResourceUsage[i])
			C.setIntArrayElement(usage, C.int(usei), ci)
			fmt.Printf(",%d", usei)
		}
		fmt.Printf(") ")
		C.setIntArrayElement(capacity, C.int(capi), ci)
		C.setBoolArrayElement(ovf, C.bool(ovfi), ci)
	}
	fmt.Printf("] \n")

	if updateUsage {
		C.stateupdater_updatePE(client.updater,
			C.CString(node.ID),
			capacity,
			ovf,
			usage,
			C.int(numResources))
	} else {
		C.stateupdater_updatePE(client.updater,
			C.CString(node.ID),
			capacity,
			ovf,
			nil,
			C.int(numResources))
	}

	return true, nil
}

// StateUpdatePod : Update the state of a pod
func (client *Client) StateUpdatePod(podState *PodState) error {

	fmt.Printf("===> StateUpdatePod: Updating pod %s on node %s with demand=[ ", podState.ID, podState.NodeID)
	numResources := len(podState.ResourceDemand)
	if numResources == 0 {
		fmt.Printf("null ] \n")
		return fmt.Errorf("StateUpdatePod: failed, no resource demands specified")
	}

	demand := C.makeIntArray(C.int(numResources))
	defer C.freeIntArray(demand)

	for i := 0; i < numResources; i++ {
		ci := C.int(i)
		di := int32(podState.ResourceDemand[i])
		fmt.Printf("%d ", di)
		C.setIntArrayElement(demand, C.int(di), ci)
	}
	fmt.Printf("] \n")

	C.stateupdater_updateLE(client.updater,
		C.CString(podState.ID),
		C.CString(podState.NodeID),
		demand,
		C.int(numResources))

	return nil
}

// StateUpdatePrint : print the state updater
func (client *Client) StateUpdatePrint() {
	C.stateupdater_print(client.updater)
	C.fflush(nil)
}

// GetPodRanks :
func (client *Client) GetPodRanks(pr *PodRequest) map[string]float64 {

	numResources := client.GetNumResources()
	cNum := C.int(numResources)

	system := (*C.cloudsystem)(unsafe.Pointer(C.stateupdater_getupdated(client.updater)))
	numNodes := int(C.cloudsystem_getnumPEs(system))
	rankMap := make(map[string]float64)
	nameList := make([]string, numNodes)

	for i := 0; i < numNodes; i++ {
		namei := C.GoString(C.cloudsystem_getPEname_byindex(system, C.int(i)))
		nameList[i] = namei
	}

	demand := C.makeIntArray(cNum)
	defer C.freeIntArray(demand)
	for i := 0; i < numResources; i++ {
		di := int32(pr.ResourceDemand[i])
		C.setIntArrayElement(demand, C.int(di), C.int(i))
	}
	lec := C.le_create(C.CString(pr.ID), demand, cNum)

	cRanks := C.evaluator_eval(client.eval, unsafe.Pointer(lec))
	C.fflush(nil)
	fmt.Printf("Node objective values = [ ")
	for i := 0; i < numNodes; i++ {
		rankMap[nameList[i]] = float64(C.float(C.getFloatArrayElement(cRanks, C.int(i))))
		fmt.Printf("(%s,%f) ", nameList[i], rankMap[nameList[i]])
	}
	fmt.Printf(" ] \n")

	C.le_destroy(lec)
	return rankMap

}
