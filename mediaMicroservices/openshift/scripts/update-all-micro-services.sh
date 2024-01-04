#!/bin/bash

cd $(dirname $0)/..

NS="ai4cloudops-f7f10d9"

for service in *service.yaml
do
  ./scripts/update-micro-service.sh --micro-service=$service --namespace=${NS}
done

cd -
