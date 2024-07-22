# cri-o in kind

https://kubernetes.io/blog/2022/12/05/forensic-container-checkpointing-alpha/
https://sysdig.com/blog/forensic-container-checkpointing-dfir-kubernetes/
https://criu.org/Main_Page
Inspired by: https://gist.github.com/aojea/bd1fb766302779b77b8f68fa0a81c0f2

Enable the feature gate: https://kubernetes.io/docs/reference/command-line-tools-reference/feature-gates/

```
# Create cluster
kind create cluster --image kindnode/crio:1.18 --name container-checkpoiting --config feature-gates.yaml

# Join host node
docker exec -it container-checkpointing-control-plane /bin/bash

# Check feature gate is enabled.
root@container-checkpointing-control-plane:/# ps aux | grep Checkpoint
root         487  1.3  0.1 1308588 106428 ?      Ssl  08:07   0:02 kube-controller-manager --allocate-node-cidrs=true <...> --feature-gates=ContainerCheckpoint=true,KubeletInUserNamespace=true <...>

# View kubelet logs
$ journalctl -u kubelet

$ crictl --runtime-endpoint unix:///run/crio/crio.sock pull registry.k8s.io/kube-apiserver:v1.30.0
```
