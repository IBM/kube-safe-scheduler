package main

import (
	"context"
	"flag"
	"fmt"
	"github.com/coreos/etcd/client"
	apiv1 "k8s.io/api/core/v1"
	"k8s.io/apimachinery/pkg/watch"
	"k8s.io/client-go/rest"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"time"

	ETCDClient "go.etcd.io/etcd/client"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	_ "k8s.io/client-go/plugin/pkg/client/auth/oidc"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	node_name_list []string

	rlns = "default"
	rllabel = "podtype"
	weight = 0.1
	node_vals = map[string]float64{}
	pod_vals = map[string]map[string]float64{}

	running_reward = 1.0
	unseen_reward = 0.5
	failed_reward = 0.0

	MTTF = 600
)

func main() {

	// creates the in-cluster config
	log.Println("[rl_monitor]Let's try in-cluster config first!\n")
	config, err := rest.InClusterConfig()

	var cfg ETCDClient.Config
	var clientset *kubernetes.Clientset
	if err != nil {
		// panic(err.Error())
		log.Println("[rl_monitor]Failed to use in-cluster config, use out-cluster config instead for debugging purpose!")
		kube_config_path := get_kube_config_path()

		var kubeconfig *string
		kubeconfig = flag.String("kubeconfig", kube_config_path, "(optional) absolute path to the kubeconfig file")
		flag.Parse()

		// use the current context in kubeconfig
		config, err = clientcmd.BuildConfigFromFlags("", *kubeconfig)
		if err != nil {
			panic(err.Error())
		}

		// creates the clientset
		clientset, err = kubernetes.NewForConfig(config)
		if err != nil {
			panic(err.Error())
		}

		etcd_endpoint, _ := clientset.CoreV1().Services(rlns).Get("rl-etcd-client-service-lb", metav1.GetOptions{})
		etcd_endpoint_str := "http://" + etcd_endpoint.Status.LoadBalancer.Ingress[0].IP + ":" + fmt.Sprintf("%d", etcd_endpoint.Spec.Ports[0].Port)
		cfg = ETCDClient.Config{
			Endpoints:               []string{etcd_endpoint_str},
			Transport:               client.DefaultTransport,
			// set timeout per request to fail fast when the target endpoint is unavailable
			HeaderTimeoutPerRequest: time.Second,
		}

	} else {
		// creates the clientset
		clientset, err = kubernetes.NewForConfig(config)
		if err != nil {
			panic(err.Error())
		}

		// creates the in-cluster config for rl-etcd-client
		cfg = ETCDClient.Config{
			Endpoints:               []string{"http://rl-etcd-client:2379"},
			Transport:               client.DefaultTransport,
			// set timeout per request to fail fast when the target endpoint is unavailable
			HeaderTimeoutPerRequest: time.Second,
		}

	}

	// Get etcd api.
	etcdcl, err := ETCDClient.New(cfg)
	if err != nil {
		log.Fatal(err)
	}
	kapi := ETCDClient.NewKeysAPI(etcdcl)

	log.Println("[rl_monitor]kubeRL rl-monitor started. Initializing the reward cache rl-etcd!")
	init_rewards(clientset, kapi)

	// Run a pod event watcher with functions to write the rewards.
	watch_pod_rewards(clientset, kapi)
}

func init_rewards(kclient *kubernetes.Clientset, kapi ETCDClient.KeysAPI) {
	nodes, err := kclient.CoreV1().Nodes().List(metav1.ListOptions{})
	if err != nil {
		log.Printf(err.Error())
		return
	}

	node_name_list = make([]string, len(nodes.Items))
	for i := 0; i < len(nodes.Items); i += 1 {
		nodename := nodes.Items[i].GetName()
		node_name_list[i] = nodename
		node_vals[nodename] = init_if_not_exists(kapi, "/nodes/" + nodename, running_reward)
		log.Printf("[rl_monitor]Initial reward on node %s is %.4f!\n", nodename, node_vals[nodename])
	}

	pods, err := kclient.CoreV1().Pods(rlns).List(metav1.ListOptions{})
	if err != nil {
		log.Print(err.Error())
		return
	}
	log.Printf("[rl_monitor]There are %d pods in the cluster\n", len(pods.Items))

	for i := 0; i < len(pods.Items); i += 1 {
		podname := pods.Items[i].GetName()
		podtype := get_podtype(&pods.Items[i])
		log.Printf("[rl_monitor]Pod name: %s, Pod type: %s, Namespace: %s, Node: %s, Restart Policy: %s \n", podname, podtype, pods.Items[i].GetNamespace(), pods.Items[i].Spec.NodeName, pods.Items[i].Spec.RestartPolicy)
		pod_node := pods.Items[i].Spec.NodeName
		init_new_pod(kapi, podtype, pod_node, node_name_list)
	}
}

func init_new_pod(kapi ETCDClient.KeysAPI, podtype string, pod_node string, node_name_list []string) {
	pod_vals[podtype] = map[string]float64{}
	pod_vals[podtype]["all"] = init_if_not_exists(kapi, "/pods/" + podtype + "/all", running_reward)

	for j := 0; j < len(node_name_list); j += 1 {
		pod_vals[podtype][node_name_list[j]] = init_if_not_exists(kapi, "/pods/" + podtype + "/" + node_name_list[j], running_reward)
		/*if (pod_node == node_name_list[j]) {
			pod_vals[podtype][node_name_list[j]] = init_if_not_exists(kapi, "/pods/" + podtype + "/" + node_name_list[j], running_reward)
		} else {
			pod_vals[podtype][node_name_list[j]] = init_if_not_exists(kapi, "/pods/" + podtype + "/" + node_name_list[j], unseen_reward)
		}*/
		log.Printf("[rl_monitor]Initializing reward for pod %s on node %s as %.4f!\n", podtype, node_name_list[j], pod_vals[podtype][node_name_list[j]])
	}
}

func init_if_not_exists(kapi ETCDClient.KeysAPI, etcd_key string, reward float64) float64 {
	var etcd_val string
	var val float64

	etcd_val = get_key_value(kapi, etcd_key)

	if etcd_val != "" {
		val, _ = strconv.ParseFloat(etcd_val, 64)
	} else {
		val = reward
		etcd_val = fmt.Sprintf("%f", val)
		write_key_value(kapi, etcd_key, etcd_val)
	}
	return val
}

func write_key_value(kapi ETCDClient.KeysAPI, keyStr string,  val string) {
	resp, err := kapi.Set(context.Background(), keyStr, val, nil)
	if err != nil {
		log.Print(err)
	} else {
		// print common key info
		log.Printf("Reward of %q is set to %q\n", resp.Node.Key, resp.Node.Value)
	}
}

func get_key_value(kapi ETCDClient.KeysAPI, etcd_key string) string{
	obj := ""

	resp, err := kapi.Get(context.Background(), etcd_key, nil)
	if err != nil {
		log.Printf("Key %s does not exist!\n", etcd_key)
	} else {
		// print common key info
		// log.Printf("Obtained map with key of %q and value of %q\n", resp.Node.Key, resp.Node.Value)
		obj = resp.Node.Value
	}
	return obj
}

func watch_pod_rewards(clientset *kubernetes.Clientset, kapi ETCDClient.KeysAPI) {
	event_watcher, err := clientset.CoreV1().Pods(rlns).Watch(metav1.ListOptions{})
	if err != nil {
		log.Fatal(err)
	}

	for {
		e := <- event_watcher.ResultChan()
		if e.Object == nil {
			return
		}
		pod, ok := e.Object.(*apiv1.Pod)
		if !ok {
			continue
		}

		podtype := get_podtype(pod)
		podnode := pod.Spec.NodeName

		log.Printf("[rl_monitor]Processing %s event for pod %s, which is with type %s on node %s and with phase %s\n", e.Type, pod.GetName(), podtype, podnode, pod.Status.Phase)
		if (podtype != "") && (e.Type != watch.Deleted) {
			switch pod.Status.Phase {
			case apiv1.PodFailed:
				update_pod_node_reward(kapi, podtype, podnode, failed_reward)
			case apiv1.PodRunning:
				updates, container_restart_reward := get_container_restarting_reward(pod)
				if updates {
					update_pod_node_reward(kapi, podtype, podnode, container_restart_reward)
				}
			case apiv1.PodSucceeded:
				// update_pod_node_reward(kapi, podtype, podnode, running_reward)
				continue
			case apiv1.PodUnknown:
				update_pod_node_reward(kapi, podtype, podnode, failed_reward)
			case apiv1.PodPending:
				continue
			}
		}
	}
}

/**
 * Obtain the pod type via the "podtype" or "app" or "name" labels, otherwise, return the pod name.
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


func get_container_restarting_reward(pod *apiv1.Pod) (bool, float64) {
	updates := true
	container_restarting_reward := running_reward

	for _, cst := range pod.Status.ContainerStatuses {
		if (cst.State.Running != nil) && (cst.LastTerminationState.Terminated == nil) {
			continue
		} else {
			if (cst.State.Terminated != nil) {
				updates = true

				if (cst.LastTerminationState.Terminated != nil) {
					time2terminated := cst.State.Terminated.StartedAt.Second() - cst.LastTerminationState.Terminated.FinishedAt.Second()
					if (time2terminated > MTTF) {
						container_restarting_reward = unseen_reward
					} else {
						container_restarting_reward = failed_reward
					}
				} else {
					container_restarting_reward = unseen_reward
				}
			} else {
				updates = false
			}
		}
	}

	return updates, container_restarting_reward
}

func update_pod_node_reward(kapi ETCDClient.KeysAPI, podtype string, nodename string, reward float64) {
	if _, ok := node_vals[nodename]; !ok{
		node_name_list = append(node_name_list, nodename)
		node_vals[nodename] = init_if_not_exists(kapi, "/nodes/" + nodename, running_reward)
	}
	node_vals[nodename] = (1 - weight) * node_vals[nodename] + weight * reward
	write_key_value(kapi, "/nodes/" + nodename, fmt.Sprintf("%f", node_vals[nodename]))

	if _, ok := pod_vals[podtype]; !ok{
		init_new_pod(kapi, podtype, nodename, node_name_list)
	}
	pod_vals[podtype][nodename] = (1 - weight) * pod_vals[podtype][nodename] + weight * reward
	write_key_value(kapi, "/pods/" + podtype + "/" + nodename, fmt.Sprintf("%f", pod_vals[podtype][nodename]))

	pod_vals[podtype]["all"] = (1 - weight) * pod_vals[podtype]["all"] + weight * reward
	write_key_value(kapi, "/pods/" + podtype + "/all", fmt.Sprintf("%f", pod_vals[podtype]["all"]))
	log.Printf("[rl_monitor]Updated reward for %s on %s with reward %f.\n", podtype, nodename, reward)
}


func get_kube_config_path() string {
	if h := os.Getenv("KUBECONFIG"); h != ""{
		return h
	}

	if h := os.Getenv("HOME"); h != "" {
		return filepath.Join(h, ".kube", "config")
	}

	return filepath.Join(os.Getenv("USERPROFILE"),".kube", "config") // windows
}