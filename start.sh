#!/usr/bin/env bash

kind delete cluster --name container-checkpointing
./kind-crio/crio.sh
kind create cluster --image quay.io/sbaumer/kindest-node-crio:1.30.0 --name container-checkpointing --config cluster-config.yaml

