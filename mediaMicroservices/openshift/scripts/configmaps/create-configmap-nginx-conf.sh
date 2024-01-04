#!/bin/bash

cd $(dirname $0)/../..

oc create cm configmap-nginx-conf   --from-file=configmaps/nginx.conf  -n ai4cloudops-f7f10d9

cd -
