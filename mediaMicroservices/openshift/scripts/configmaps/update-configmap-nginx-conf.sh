#!/bin/bash

cd $(dirname $0)/../..

NS="ai4cloudops-f7f10d9"

oc create cm configmap-nginx-conf   --from-file=configmaps/nginx.conf  --dry-run --save-config -o yaml -n ${NS} | oc apply -f - -n ${NS}

cd -
