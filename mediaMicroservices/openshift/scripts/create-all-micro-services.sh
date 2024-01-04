#!/bin/bash

cd $(dirname $0)/..

NS="ai4cloudops-f7f10d9"

oc create namespace ${NS}

for service in *service.yaml
do
  oc apply -f $service --namespace ${NS}
done

cd -
