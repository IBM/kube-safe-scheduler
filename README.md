# kube-safe-scheduler


Scheduler extension for Kubernetes, together with node annotator that collects statistics from Prometheus (or Metric server) to be used by the scheduling algorithm. The Safe Scheduler performs risk-aware (safe) allocation of resources, based on actual and predicted resource usage statistics of each node in the cluster, hence leading to safer placement of workload avoiding noisy neighbors, and reducing probability of evictions. It also can be configured to perform risk-aware over-commitment leading to better cluster utilization. The risk calculation minimizes the probability of exceeding the available resources, thus the name "Safe Scheduler".

We will use this project as incubator for new Kube scheduler extension ideas. One such idea brewing is to avoid nodes the pod is likely to fail on based on historical evidences.

