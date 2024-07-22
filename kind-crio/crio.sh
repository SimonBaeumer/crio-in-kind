#!/usr/bin/env bash
set -eo pipefail
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

docker build -t quay.io/sbaumer/kindest-node-crio:1.30.0 $SCRIPT_DIR
docker push quay.io/sbaumer/kindest-node-crio:1.30.0