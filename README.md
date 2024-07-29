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
$ journalctl -u crio

$ crictl --runtime-endpoint unix:///run/crio/crio.sock pull registry.k8s.io/kube-apiserver:v1.30.0
E0726 13:40:42.726916     714 remote_image.go:180] "PullImage from image service failed" err=<
	rpc error: code = Unknown desc = copying system image from manifest list: writing blob: adding layer with blob "sha256:0c6c1ee970856d051398b9bdfbd829083b7fa79aeb59435f4dafae3837ba8948": processing tar file(time="2024-07-26T13:40:42Z" level=warning msg="Failed to read CAP_SYS_ADMIN presence for the current process"
	time="2024-07-26T13:40:42Z" level=warning msg="Failed to read current user namespace mappings"
	operation not permitted): exit status 1
 > image="registry.k8s.io/kube-apiserver:v1.30.0"
FATA[0002] pulling image: copying system image from manifest list: writing blob: adding layer with blob "sha256:0c6c1ee970856d051398b9bdfbd829083b7fa79aeb59435f4dafae3837ba8948": processing tar file(time="2024-07-26T13:40:42Z" level=warning msg="Failed to read CAP_SYS_ADMIN presence for the current process"
time="2024-07-26T13:40:42Z" level=warning msg="Failed to read current user namespace mappings"
operation not permitted): exit status 1 

root@container-checkpointing-control-plane:/# echo 1 > /proc/self/oom_score_adj 
bash: echo: write error: Permission denied

❯ docker run --rm -it --entrypoint /bin/bash --privileged quay.io/sbaumer/kindest-node-crio:1.30.0
root@0a2a40b4197d:/# exit
exit
❯ docker run --rm --security-opt seccomp=unconfined -it --entrypoint /bin/bash --privileged quay.io/sbaumer/kindest-node-crio:1.30.0
root@bb6ab4c43a43:/# echo 1 > /proc/self/oom_score_adj
bash: echo: write error: Permission denied
root@bb6ab4c43a43:/# 

# set ENV _CRIO_ROOTLESS=1 in Dockerfile
# crio starts

    
```

### Debug

- Has the current process the required capabilities (`CAP_SYS_ADMIN`)?

```
$ cat /proc/$$/status | grep Cap
CapInh:	0000000000000000
CapPrm:	000001ffffffffff
CapEff:	000001ffffffffff
CapBnd:	000001ffffffffff
CapAmb:	0000000000000000

$ capsh --decode=000001ffffffffff
0x000001ffffffffff=cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_kill,cap_setgid,cap_setuid,cap_setpcap,cap_linux_immutable,cap_net_bind_service,cap_net_broadcast,cap_net_admin,cap_net_raw,cap_ipc_lock,cap_ipc_owner,cap_sys_module,cap_sys_rawio,cap_sys_chroot,cap_sys_ptrace,cap_sys_pacct,cap_sys_admin,cap_sys_boot,cap_sys_nice,cap_sys_resource,cap_sys_time,cap_sys_tty_config,cap_mknod,cap_lease,cap_audit_write,cap_audit_control,cap_setfcap,cap_mac_override,cap_mac_admin,cap_syslog,cap_wake_alarm,cap_block_suspend,cap_audit_read,cap_perfmon,cap_bpf,cap_checkpoint_restore
```

Result: Has `cap_sys_admin` set.

- Is the user mapped correctly? (user namespace) https://www.schutzwerk.com/en/blog/linux-container-namespaces04-user/


https://github.com/cri-o/cri-o/issues/4902
https://github.com/containers/podman/issues/13449