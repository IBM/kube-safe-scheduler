from kubernetes import client, config
from datetime import datetime
from ast import literal_eval
from quantity import parse_quantity
import time
import os
import copy


resName = ("cpu", "memory", "pods", "gpu", "ephemeral-storage")

class Agent:

    client = None
    cluster_config = "in-cluster"
    freq = 60 # frequency of annotation. The unit is in seconds


    def __init__(self):
        self.set_env()


    def set_env(self):
        print ("Setting up environment ...")
        if os.environ.get('CLUSTER_CONF') is not None:
            self.cluster_config = os.environ['CLUSTER_CONF']
        print("Using cluster_config = " + self.cluster_config)

        if os.environ.get('ANNOTATE_FREQ') is not None:
            freq = int(os.environ['ANNOTATE_FREQ'])
            if freq > 0:
                self.freq = freq
            else:
                print ('Environment variable ANNOTATE_FREQ not set properly: {0}'.format(freq))
        print("using freq = " + str(self.freq))


    def create_client(self):
        print ("Creating Kubernetes client ...")
        if self.cluster_config == 'out-of-cluster':
            try:
                config.load_kube_config()
            except Exception as e:
                print("Error loading config: {0}".format(e))
                print("Attempting to use config file kubectl.conf")
                try:
                    config.load_kube_config(config_file='kubectl.conf')
                except Exception as e:
                    print("Error loading config file: {0}".format(e))
                    return False
            print("Using out-of-cluster config")
        else:
            try:
                config.load_incluster_config()
            except Exception as e:
                print("Error loading in-cluster config: {0}".format(e))
                return False
            print("Using in-cluster config")
        self.client = client.CoreV1Api()
        return True


    def get_node_capacity(self):
        nodeCap = dict()
        try:
            nodes = self.client.list_node()
        except Exception as e:
            print(e)
            return nodeCap
        for n in nodes.items:
            for status in n.status.conditions:
                if status.status == "True" and status.type == "Ready":
                    node_name = copy.copy(n.metadata.name)
                    alloc = n.status.allocatable
                    capacity = [0] * len(resName)
                    for i in range(len(resName)):
                        name = resName[i]
                        if name in alloc.keys():
                            capacity[i] = parse_quantity(alloc[name])
                        # CPU units in Milli
                    capacity[0] = int(capacity[0] * 1000)
                    nodeCap[node_name] = capacity
        return nodeCap


    def find_node_ip(self, item):
        for i in item.status.addresses:
            if i.type == "InternalIP":
                return i.address
        return


    def get_node_allocation(self):
        nodeAlloc = dict()
        ret = self.client.list_pod_for_all_namespaces(watch=False)
        for pod in ret.items:
            node_name = copy.copy(pod.spec.node_name)
            if (node_name != None and pod.status.phase == "Running"):
                if node_name not in nodeAlloc:
                    nodeAlloc[node_name] = [0] * len(resName)
                alloc = [0] * len(resName)
                for c in pod.spec.containers:
                    req = c.resources.requests
                    if req == None:
                        continue
                    for i in range(len(resName)):
                        name = resName[i]
                        if name in req.keys():
                            alloc[i] += parse_quantity(req[name])
                # CPU units in Milli
                alloc[0] = int(alloc[0] * 1000)
                # pod count is one
                if len(resName) > 2:
                    alloc[2] = 1
                for i in range(len(resName)):
                    nodeAlloc[node_name][i] += alloc[i]
        return nodeAlloc


    def annotate_nodes(self, nodeCap, nodeAlloc):
        for node_name in nodeCap:
            cap = nodeCap[node_name]
            alloc = [0] * len(resName)
            if node_name in nodeAlloc:
                alloc = nodeAlloc[node_name]
            free = [0] * len(resName)
            for i in range(len(resName)):
                free[i] = cap[i] - alloc[i]
                if free[i] < 0:
                    free[i] = 0

            print(node_name + ": ", end =" ")
            for i in range(len(resName)):
                print(resName[i] + "=[" + str(cap[i]) + ", " + str(alloc[i]) + ", " + str(free[i]) + "];  ", end =" ")
            print()

            annot = dict()
            for i in range(len(resName)):
                kf = 'free-' + resName[i]
                vf = str(free[i])
                kr = 'requested-' + resName[i]
                vr = str(alloc[i])
                annot[kf] = vf
                annot[kr] = vr

            body = {
                "metadata": {
                    "annotations": annot
                }
            }

            try:
                self.client.patch_node(node_name, body)
            except Exception as e:
                print(e)
                return


    def main(self):
        if not self.create_client():
            print ("Failed creating client!")
            return
        print ("Client created successfully!")
        print("Format is node_name: resource[allocatable, requested, available]")
        print("")
        while True:
            print(datetime.now())

            nodeCap = self.get_node_capacity()
            nodeAlloc = self.get_node_allocation()
            self.annotate_nodes(nodeCap, nodeAlloc)

            print("", flush=True)
            time.sleep(self.freq)


if __name__ == '__main__':

    ag = Agent()
    ag.main()



