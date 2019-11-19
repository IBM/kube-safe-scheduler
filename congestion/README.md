# Congestion Agent: Scheduling based on node congestion

The Congestion agent scheduler extender together with a node annotator, which collects statistics about node congestion, provides a Kubernetes scheduling solution that is influenced by additional node congestion measure.

## Description

This is an extender to a default Kubernetes scheduler which provides a priority function called *'congestion'*. A node is continuously annotated with a value representing a form of congestion on the node. This may be due to measured load on the node and/or networking congestion in and out of the node. The key for the annotation is *'congestion-factor'* and the value is an integer between zero and 100 (or another maximum value provided by the environment variable 'CONGESTION_FACTOR_MAX'). The higher the value the more congestion there is on the node. A missing congestion value means zero congestion. The intent of this priority function is to have the scheduler prefer nodes with low congestion values. In order to activate the *'congestion'* priority function, one needs to add in the [extender](../yamlfiles/extender.congestion.yaml) deployment file:

`"prioritizeVerb": "priorities/congestion",`

 to the *ConfigMap policy.cfg* definition, under *"extenders"*.

