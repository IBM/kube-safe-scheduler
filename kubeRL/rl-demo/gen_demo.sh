#!/bin/bash

# Get a list of nodes from the cluster
read -a nodes <<< $(kubectl get nodes|cut -d' ' -f1|tail -n +2)

# Generate the demo script
nodecount=${#nodes[@]}
if [[ $nodecount < 2 ]]; then
	echo "You cluster needs at least 2 nodes to test the rl-scheduler-extender!"
	exit -1
fi

sedcmd="s/NODE1/${nodes[0]}/g;s/NODE2/${nodes[1]}/g"
echo $sedcmd
sed $sedcmd test_host.template > test_host.sh 
