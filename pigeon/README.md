# Pigeon Agent: A policy based scheduler extender

The Pigeon scheduler extender agent optimizes particular policy objectives when scheduling a pod. The name pigeon is inspired from pigeon holing, where pigeons select holes in a pigeonhole structure in a collective and organized way, in analogy to bin-packing of pods on nodes in a cluster. 

## Description

The pigeon agent uses a library (libpigeon, written in C) to solve an optimization (minimization) problem with a given objective function. The priority function of the extender, called *pigeon-holing*, returns values inversly proportional to the values of the objective function. (Note that larger values of priority are more preferable.) Currently, we support the following policy objectives.

- <u>Adaptive Bin Packing (*A_BINPACK*)</u>

  A novel algorithm for bin packing called *ABP* has been introduced and detailed in [reference 1](#icac19). A brief presentation is provided [here](../docs/abp.pdf). The problem is that variability in pod sizes (in term of requested resources) dictates an optimal way for placing pods on nodes. The extreme cases are (1) equal sized pods and (2) a mixture of really small and really large pods. In the first case, it is better to spread the pods among the nodes, placing on the least allocated node. Whereas, in the second case, it is better to pack the pods on nodes, placing on the most allocated node, so as to be able to place large pods when they arrive. Given a spectrum of pod size variability, the following questions arise:  How to define and measure variability in pod sizes?  And, what is the scheme for placing the pods accordingly? 

  The *ABP* algorithm learns the variability in pod sizes and adjusts the scheme for their placement in an adaptive way. Thus, selecting the *A_BINPACK* policy relieves us from deciding whether to spread, pack, or any other scheme in between. Further, the *ABP* algorithm deals with multiple resources (CPU, memory, GPU, ...) in a seamless way. *ABP* analyzes the correlation of sizes (requests) in multiple dimensions (resources). The objective function is the difference between the observed variability in pod sizes and the variability in node allocation in the cluster.

- <u>Spreading (*LOAD_BALANCE*)</u>

  The *LOAD_BALANCE* policy minimizes the variability in resource allocation in the nodes in the cluster. The objective function is the standard deviation of the allocation of the (configurable) prime resource (CPU, by default).

- <u>Packing (*CONSOLIDATE*)</u>

  The *CONSOLIDATE* policy does the opposite. The objective function is the negation of the standard deviation of the allocation of the (configurable) prime resource (CPU, by default).

## Node allocation

The amount of resources allocated on a node is the aggregation of requested resources of all containers within all pods running on the node. The pigeon agent scheduler extender needs to obtain node allocation information before evaluating its priority function, in relation to scheduling a pod. Since node allocation information is not included in the arguments when the scheduler calls the extender, it will have to be obtained through other means. There are two common ways to obtain such information.

- Internal method: Using this method, the agent would use a Kubernetes client to query the API server for information about all nodes, all pods, all containers running in pods, and the amount of requested resources per container. This operation has to be performed before scheduling each pod.
- External method: A node annotator, external to the scheduler extender, uses a Kubernetes client as per the internal method, and periodically annotates the nodes with calculated node allocation information. The agent would then consult the node annotations when scheduling a pod.

Each mehod has its pros and cons. The internal method results in fresh data and does not require additional external components, but may cause an overhead due to potentially excessive access to the API server. And, the external method has an adjustable (through the choice of update frequency) and lower overhead, at the expense of potentially stale data.

We chose to implement the external method. A node allocation annotator, called [k8s-assessor](../node-annotator/allocation/), is provided. The frequency of annotation is adjustable through an environment variable.

## Configuration

A configuration [file](../pigeon_c/pigeon.cfg) is provided to specify the following configuration variables.

- **policyObjective**: Current choices are *A_BINPACK* (default), *LOAD_BALANCE*, and *CONSOLIDATE*.
- **policyResourceIndex**: This is the index of the prime resource. The supported resources are (in order): CPU, memory, number of pods, GPU, and storage. Hence, the index of resources is 0, 1, ..., respectively. Not all resources have to be considered when placing pods. By default, the first two resources are considered. But, one may override this choice by setting an environment variable as described below. Currently, the choice of the number of resources, say n, dictates that resources with indices 0, 1, ..., n-1 are all considered. Extension to selective resources is straightforward.

In addition, the following environment variables may be used.

- **POLICY_OBJECTIVE**: Supported values are *A_BINPACK*, *LOAD_BALANCE*, and *CONSOLIDATE*. (Default *A_BINPACK*)
- **NUM_RESOURCES**: Supported values are in the range [1,5], as outlined above. (Default 2)

### References

<a name="icac19">

1. A. N. Tantawi and M. Steinder, "Autonomic Cloud Placement of Mixed Workload: An Adaptive Bin Packing Algorithm," 2019 IEEE International Conference on Autonomic Computing (ICAC), Umea, Sweden, 2019, pp. 187-193.

</a>