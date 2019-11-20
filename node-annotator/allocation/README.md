# k8s-assessor

An agent which goes over the nodes in a Kubernetes cluster and calculates the total requested resources. We consider the following resources: "cpu", "memory", "pods", "gpu", and "ephemeral-storage". The total resource requested quantities are annotated as 'requested-xxx', where 'xxx' is the resource name. Further, the agent computes the available allocatable resources and annotates the nodes with 'free-xxx' quantities. The units for CPU are in milli-cores and for memory and storage in bytes.

The k8s-assessor is written in Python 3 and runs either externally to the cluster or internally in a pod. 

### External agent

Make sure to install the kubernetes package.

```
pip install kubernetes
```

The Kubernetes configuration file used is either the one set in the environment variable KUBECONFIG or (if not set) the default location *~/.kube/config*.

Then, run the agent.

```
export CLUSTER_CONF=out-of-cluster

python k8s-assessor.py
```



### Internal agent

Build the image.

```
docker build . -t k8s-assessor:0.0
```

Upload the image and run as a pod.

```
kubectl create -f k8s-assessor.yaml
```

If RBAC is enabled in the cluster, then you may use the following.

```
kubectl create -f k8s-assessor-rbac.yaml
```



A sample output is

```
Setting up environment ...
Using cluster_config = out-of-cluster
using freq = 60
Creating Kubernetes client ...
Using out-of-cluster config
Client created successfully! 
Format is node_name: resource[allocatable, requested, available]

2019-10-30 10:29:56.265725
minikube:  cpu=[2000, 855, 1145];   memory=[2033401856, 272629760, 1760772096];   pods=[110, 12, 98];   gpu=[0, 0, 0];   ephemeral-storage=[17361141760, 0, 17361141760];  

```



A sample annotation is

```
Annotations:        free-cpu: 1145
                    free-ephemeral-storage: 17361141760
                    free-gpu: 0
                    free-memory: 1760772096
                    free-pods: 98
                    requested-cpu: 855
                    requested-ephemeral-storage: 0
                    requested-gpu: 0
                    requested-memory: 272629760
                    requested-pods: 12
```



The frequency of running the agent may be altered through the environment variable ANNOTATE_FREQ (units in seconds). The default is 60.

```
export ANNOTATE_FREQ=10
```



##### Notes:

- The quantity.py script is added here since it is not included in older kubernetes client versions. It is needed to parse memory quantities and make the conversion to byte units.
- May not run correctly with Python 2.
- The k8s-assessor.py is built on the node-annotator of the kube-safe-scheduler extender project.
- An alternative approach is to *grep* the block entitled *Allocated resources:* obtained from *kubectl describe node* and parsing it (or by parsing the obtained json file).

