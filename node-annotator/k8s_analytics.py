
from kubernetes import client, config
from prom_query import FetchData
import time
import os
import math
from ast import literal_eval

class Coordinator:

    v1 = None
    prom_address = '127.0.0.1'
    prom_port = '9090'
    node_exporter_port = '9100'
    cluster_config = "in-cluster"
    requested_usage_only = False
    freq = 60 # frequency of annotation. The unit is in seconds
    forecast = False

    def __init__(self):
        self.set_env()
        print ("the configuration used is: \nprom_address = {0}, \nprom_port = {1}, \nnode_exporter_port = {2}, \ncluster_config = {3}, \nrequested_usage_only = {4}, \nfreq = {5} , \nforecast = {6}".
               format(self.prom_address, self.prom_port, self.node_exporter_port, self.cluster_config, self.requested_usage_only, self.freq, self.forecast))

    def annotate_node(self, node_name, node_ip, node_requested, node_allocatable, requested_memory_usage):

        mean, std, min, predicted = FetchData.fetch_data_range('http://{0}:{1}'.format(self.prom_address,self.prom_port),
                              'node_memory_MemAvailable_bytes{{instance="{0}:{1}"}}'.format(node_ip,self.node_exporter_port),
                                                               (int(time.time() - 86400)),
                              '15s', int(time.time()), node_requested, node_allocatable, self.forecast)

        if predicted == 0:
            predicted = "N/A"

        mem_free = FetchData.fetch_data_scalar('http://{0}:{1}'.format(self.prom_address, self.prom_port),
                              'node_memory_MemAvailable_bytes{{instance="{0}:{1}"}}'.format(node_ip, self.node_exporter_port))


        icpu_std = FetchData.fetch_data_scalar('https://{0}:{1}/prometheus'.format(self.prom_address,self.prom_port),
                                               'sum by (instance) (stddev_over_time(node_cpu_seconds_total{{instance="{0}:{1}",mode="idle"}}[5m])/(5*60))*1000'.format(node_ip,self.node_exporter_port))


        
        icpu_free = FetchData.fetch_data_scalar('https://{0}:{1}/prometheus'.format(self.prom_address,self.prom_port),
                                                'sum by (instance)(irate(node_cpu_seconds_total{{instance="{0}:{1}",mode="idle"}}[5m]))*1000'.format(node_ip,self.node_exporter_port))

        body = {
            "metadata": {
                "annotations": {
                    "mean-free-memory": str(mean),
                    "std-free-memory": str(std),
                    "forcasted-free-memory": str(predicted),
                    "mean-free-cpu": str(float(icpu_free)),
                    "std-free-cpu": str(float(icpu_std)),
                    "free_memory": str(mem_free),
                    "min_free_memory": str(min),
                    "reqested_memory": str(requested_memory_usage),
                    "allocatable_memory": str(node_allocatable)
                }
            }
        }

        unit_parsing = False
        if unit_parsing == True:
            body = {
                "metadata": {
                    "annotations": {
                        "mean-free-memory": str(math.floor(mean/1000000))+"Mi",
                        "std-free-memory": str(math.floor(std/1000000))+"Mi",
                        "free_memory": str(math.floor(int(mem_free)/1000000))+"Mi",
                        "min_free_memory": str(math.floor(min/1000000))+"Mi",
                        "reqested_memory": str(math.floor(requested_memory_usage/1000000))+"Mi",
                        "allocatable_memory": str(math.floor(node_allocatable/1000000))+"Mi"
                    }
                }
            }


        try:
                v1.patch_node(node_name, body)
        except Exception as e:
                print(e)
                return
        print("node {0} annotated!".format(node_name))

    def requested_memory(self,node_name):
        total_req = 0
        #get all pods
        ret = v1.list_pod_for_all_namespaces(watch=False)
        #filter out the ones not on the node
        for i in ret.items:
           if (i.status.phase == "Running") & (i.spec.node_name == node_name):
                # for each container in the pod, get the requested resource
                for c in i.spec.containers:
                    req = c.resources.requests
                    if req == None:
                        continue
                    if 'memory' in req.keys():
                        total_req += int(filter(str.isdigit,  req['memory']))
        return total_req

    def set_env(self):
        if os.environ.get('PROM_IP') is not None:
            self.prom_address = os.environ['PROM_IP']
        else:
            print ('the environment variable PROM_IP is not set, so using default: {0}'.format(self.prom_address))

        if os.environ.get('PROM_PORT') is not None:
            self.prom_port = os.environ['PROM_PORT']
        else:
            print ('the environment variable PROM_PORT is not set, so using default: {0}'.format(self.prom_port))

        if os.environ.get('NODE_EXP_PORT') is not None:
            self.node_exporter_port = os.environ['NODE_EXP_PORT']
        else:
            print ('the environment variable NODE_EXP_PORT is not set, so using default: {0}'.format(self.node_exporter_port))

        if os.environ.get('CLUSTER_CONF') is not None:
            self.cluster_config = os.environ['CLUSTER_CONF']
        else:
            print ('the environment variable Cluster_CONF is not set, so using default: {0}'.format(self.cluster_config))

        if os.environ.get('USE_FORECAST') is not None:
            if os.environ['USE_FORECAST'].lower() == "true":
                self.forecast = True
        else:
            print ('the environment variable USE_FORECAST is not set, so using default: {0}'.format(self.forecast))

        if os.environ.get('USE_REQ') is not None:
            if os.environ['USE_REQ'].lower() == "true":
                self.requested_usage_only = True
        else:
            print ('the environment variable USE_REQ is not set, so using default: {0}'.format(self.requested_usage_only))

        if os.environ.get('ANNOTATE_FREQ') is not None:
            freq = int(os.environ['ANNOTATE_FREQ'])
            if freq > 0:
                self.freq = freq
            else:
                print ('the environment variable ANNOTATE_FREQ is not set properly: {0}'.format(freq))
        else:
            print ('the environment variable ANNOTATE_FREQ is not set, so using default: {0}'.format(self.freq))

    def find_node_ip(self, item):
        for i in item.status.addresses:
            if i.type == "InternalIP":
                return i.address
        return

    def main(self):
        # Configs can be set in Configuration class directly or using helper
        # utility. If no argument provided, the config will be loaded from
        # default location.
        print ("Initializing ...")

        if self.cluster_config == 'out-of-cluster':
            try:
                config.load_kube_config(config_file='kubectl.conf')
            except Exception as e:
                print("Error loading out of cluster k8s config: {0}".format(e))
                return
            print("using out-of-cluster K8s conf")
        else:
            try:
                config.load_incluster_config()
            except Exception as e:
                print("Error loading in-cluster k8s config: {0}".format(e))
                return
            print("using in-cluster K8s conf")

        global v1
        v1 = client.CoreV1Api()

        while True:
            print("Listing Nodes with their capacity:")
            try:
                    result = v1.list_node()
            except Exception as e:
                    print(e)
                    return

            for i in result.items:
                node_ip = self.find_node_ip(i)
                print("%s\t%s CPU \t%s RAM " %
                      (node_ip, i.status.allocatable['cpu'], i.status.allocatable['memory']))
                #TODO check the unit before multiplying by 1000, I am assuming it is 'Ki'
                allocatable =  1000 * int((filter(str.isdigit, i.status.allocatable['memory'])))
                requested_memory_usage = self.requested_memory(i.metadata.name)*1000000
                if self.requested_usage_only:
                    requested_memory = requested_memory_usage
                else:
                    requested_memory = 0
                self.annotate_node(i.metadata.name, node_ip, requested_memory , allocatable, requested_memory_usage)

            time.sleep(self.freq)




if __name__ == '__main__':

    co = Coordinator()
    co.main()
