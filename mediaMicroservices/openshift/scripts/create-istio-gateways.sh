#!/bin/bash

NS="ai4cloudops-f7f10d9"

cd $(dirname $0)/..

oc apply -f networking/istio-gateway/mediamicrosvc-gateway.yaml -n ${NS}

cd -
