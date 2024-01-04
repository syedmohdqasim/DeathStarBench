#!/bin/bash

cd $(dirname $0)/../..

oc create cm configmap-gen-lua --from-file=configmaps/gen-lua -n ai4cloudops-f7f10d9

cd -
