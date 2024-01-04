#!/bin/bash

cd $(dirname $0)/..

NS="ai4cloudops-f7f10d9"

for service in *.yaml
do
  echo updating $service
  ./scripts/update-micro-service.sh --micro-service=$service --namespace=${NS}
done
echo update all configmap 
./scripts/update-all-configmap.sh

echo update destination rule
./scripts/update-destination-rule-all.sh

cd -
