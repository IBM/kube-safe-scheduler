package main

import (
	"context"
	"fmt"
	"github.com/coreos/etcd/client"
	k8s_scheduler_extender "github.com/wangchen615/k8s-scheduler-extender-example/k8s_scheduler_extender"
	"log"
	"strconv"
	"strings"
	"time"

	ETCDClient "go.etcd.io/etcd/client"
	apiv1 "k8s.io/api/core/v1"

	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

var (
	rllabel = "podtype"
)


// RLPriority : prioritize nodes with higher rewards.
var RLPriority = k8s_scheduler_extender.Prioritize{
	Name: "rl-priority",
	Func: func(pod apiv1.Pod, nodes []apiv1.Node) (*schedulerapi.HostPriorityList, error) {
		return RLPriorityFunc(pod, nodes, true)
	},
}

/**
 * Define rl priority functions
 */
func RLPriorityFunc(pod apiv1.Pod, nodes []apiv1.Node, isBalance bool) (*schedulerapi.HostPriorityList, error) {
	var cfg ETCDClient.Config

	// creates the in-cluster config for rl-etcd-client
	cfg = ETCDClient.Config{
		Endpoints:               []string{"http://rl-etcd-client.default:2379"},
		Transport:               client.DefaultTransport,
		// set timeout per request to fail fast when the target endpoint is unavailable
		HeaderTimeoutPerRequest: time.Second,
	}

	// Get etcd api.
	etcdcl, err := ETCDClient.New(cfg)
	if err != nil {
		log.Fatal(err)
	}
	kapi := ETCDClient.NewKeysAPI(etcdcl)

	// Obtain the rewards for different nodes
	podType := get_podtype(&pod)
	podObj := get_key_directory(kapi, "/pods/" + podType)

	var priorityList schedulerapi.HostPriorityList
	priorityList = make([]schedulerapi.HostPriority, len(nodes))

	for i, node := range nodes {
		log.Printf("==> RLPriority: retrieving priority for pod %s on node %s from rl-etcd \n",
			pod.Name, node.Name)

		score := podObj[node.GetName()] * schedulerapi.MaxPriority
		priorityList[i] = schedulerapi.HostPriority{
			Host:  node.Name,
			Score: int(score),
		}
	}

	return &priorityList, nil
}

/**
 * retrieve a dict from etcd by requesting the directory key value.
 */
func get_key_directory(kapi ETCDClient.KeysAPI, etcd_key string) map[string]float64{
	obj := map[string]float64{}

	resp, err := kapi.Get(context.Background(), etcd_key, nil)
	if err != nil {
		log.Printf("Key %s does not exist!\n", etcd_key)
		return nil
	}
	if resp.Node.Dir {
		fmt.Print(resp.Node.Nodes)
		for _, cur_node := range resp.Node.Nodes {
			etcd_key := cur_node.Key
			nodename := etcd_key[strings.LastIndex(etcd_key, "/")+1:]
			obj[nodename], err = strconv.ParseFloat(cur_node.Value, 64)
		}
	}

	return obj
}

/**
 * Obtain the pod type via the "podtype" or "app" or "name" labels
 */
func get_podtype(pod *apiv1.Pod) string {
	podlabels := pod.GetLabels()

	if podtype, ok := podlabels[rllabel]; ok {
		return podtype
	} else if podtype, ok = podlabels["app"]; ok {
		return podtype
	} else if podtype, ok = podlabels["name"]; ok {
		return podtype
	} else {
		log.Printf("Pod %s does not have type label", pod.GetName())
		return pod.GetName()
	}
}