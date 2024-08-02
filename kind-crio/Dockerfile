ARG IMAGE=kindest/node
ARG VERSION=1.30
ARG MINOR=0
ARG OS=xUbuntu_24.04

FROM ${IMAGE}:v${VERSION}.${MINOR}

ARG IMAGE=kindest/node
ARG VERSION=1.30
ARG MINOR=0
ARG OS=Debian_12
ARG KUBERNETES_VERSION=v1.30

ENV _CRIO_ROOTLESS=1

RUN apt-get update && apt-get install -y software-properties-common curl vim git criu lsof
RUN curl -fsSL https://pkgs.k8s.io/core:/stable:/${KUBERNETES_VERSION}/deb/Release.key | gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
RUN echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/${KUBERNETES_VERSION}/deb/ /" | tee /etc/apt/sources.list.d/kubernetes.list

RUN echo curl -fsSL https://pkgs.k8s.io/addons:/cri-o:/stable:/v${VERSION}/deb/Release.key
RUN curl -fsSL https://pkgs.k8s.io/addons:/cri-o:/stable:/v${VERSION}/deb/Release.key | gpg --dearmor -o /etc/apt/keyrings/cri-o-apt-keyring.gpg
RUN echo "deb [signed-by=/etc/apt/keyrings/cri-o-apt-keyring.gpg] https://pkgs.k8s.io/addons:/cri-o:/stable:/v${VERSION}/deb/ /" | tee /etc/apt/sources.list.d/cri-o.list

#TODO: re-enable repository
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get --option=Dpkg::Options::=--force-confdef install -y cri-o

RUN ln -s /usr/libexec/podman/conmon /usr/local/bin/conmon
RUN sed -i 's/containerd/crio/g' /etc/crictl.yaml

#COPY crio.service /etc/systemd/system/cri-o.service

COPY 02-cgroup-manager.conf /etc/crio/crio.conf.d/02-cgroup-manager.conf
COPY 03-capabilities.conf /etc/crio/crio.conf.d/03-capabilities.conf
COPY user@.service /usr/lib/systemd/system/user@.service

RUN systemctl disable containerd \
     && systemctl enable crio.service

RUN cat /etc/systemd/system/cri-o.service

RUN systemctl daemon-reload

# TODO: install criu