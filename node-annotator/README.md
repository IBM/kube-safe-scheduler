# node-annotator


node-annotator queries current CPU & memory utilization of the nodes using Prometheus deployed in the Kubernetes Cluster.  Please set these values in node-annotator.yaml to collect performance metrics into node resources

```bash
	PROMETHEUS_IP_ADDRESS
	PROMETHEUS_PORT (default is 9090)
	NODE_EXPORTER_PORT (default is 8445)
```

How to get the Prometheus IP address: Run the following command where monitoring-prometheus-xxxxx must be the correct prometheus pod name

```bash
kubectl get pods monitoring-prometheus-xxxxx -n=kube-system -o wide
```

How to get the Prometheus Port
```bash
kubectl get svc -n kube-system | grep monitoring-prometheus
```

How to get the Node Exporter Port
```bash
kubectl get svc -n kube-system | grep monitoring-prometheus-nodeexporter 


## Instructions

### 1. buid a docker image

```
$ docker build . -t node-annotator:1.13.5
```

### 2. push image into image repository (e.g. sample push to https://hub.docker.com)
```
$ docker push myrepository/node-annotator:1.13.5


```
### 3. deploy `node-annotator` in `kube-system` namespace
```
$ kubectl create -f node-annotator.yaml
```