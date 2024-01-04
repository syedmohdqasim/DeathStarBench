#!/bin/bash

cd $(dirname $0)/../..

oc create cm configmap-jaeger-config-json   --from-file=configmaps/jaeger-config.json -n ai4cloudops-f7f10d9

cd -
