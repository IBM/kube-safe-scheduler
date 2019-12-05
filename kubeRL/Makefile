USERID := $(shell cat config.env|cut -d'=' -f2)

build-demo:
	# Build rl-demo apps
	cd ./rl-demo && ./gen_demo.sh && docker build --no-cache -t $(USERID)/rl-demo:latest .
	docker push $(USERID)/rl-demo:latest
build-rl-monitor:
	cd ./rl-monitor && docker build --no-cache -t $(USERID)/rl-monitor:latest .
	docker push $(USERID)/rl-monitor:latest
build-rl-scheduler-extender:
	cd ./rl-scheduler-extender && docker build --no-cache -t $(USERID)/rl-scheduler-extender:latest .
	docker push $(USERID)/rl-scheduler-extender:latest
build-all: build-demo build-rl-monitor build-rl-scheduler-extender
clean-demo:
	# Clean demo apps
	rm ./rl-demo/test_host.sh || true
	kubectl delete -f manifests/demo-deployment.yaml || true
	kubectl delete -f manifests/test-pod.yaml || true
clean-rl-monitor:
	# Clean rl-monitor agent.
	kubectl delete -f manifests/rl-monitor-deployment.yaml || true
clean-rl-etcd:
	# Clean ETCD
	kubectl delete -f manifests/rl-etcd.yaml || true
	kubectl delete -f manifests/etcd-operator.yaml || true
	kubectl delete endpoints etcd-operator
	kubectl delete crd etcdclusters.etcd.database.coreos.com
	kubectl delete clusterrole etcd-operator
	kubectl delete clusterrolebinding etcd-operator
	# kubectl delete -f manifests/rl-etcd-client-service-lb.yaml || true
clean-rl-scheduler-extender:
	# Client scheduler extender
	kubectl delete -f manifests/rl-scheduler.yaml || true
clean-all: clean-demo clean-rl-monitor clean-rl-scheduler-extender clean-rl-etcd
deploy-rl-etcd:
	# Deploy rl-etcd cluster
	./manifests/rbac/create_role.sh
	kubectl create -f manifests/etcd-operator.yaml
	@bash -c 'sleep 15 && echo "Wait until EtcdOperator is available"'
	kubectl create -f manifests/rl-etcd.yaml
	@bash -c 'sleep 30 && echo "Wait until EtcdCluster is available"'
deploy-rl-monitor:
	# Deploy rl-monitor agent
	kubectl create -f manifests/rl-monitor-deployment.yaml
deploy-rl-scheduler-extender:
	# Deploy rl-scheduler-extender
	@bash -c 'sleep 15 && echo "Wait until rl-monitor finish initializing the reward cache rl-etcd!"'
	kubectl create -f manifests/rl-scheduler.yaml
demo:
	kubectl create -f manifests/demo-deployment.yaml
deploy-only: deploy-rl-etcd deploy-rl-monitor deploy-rl-scheduler-extender
deploy-all: clean deploy-only demo
