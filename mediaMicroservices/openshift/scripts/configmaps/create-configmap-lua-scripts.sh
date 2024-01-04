#!/bin/bash

N=ai4cloudops-f7f10d9

cd $(dirname $0)/../..

oc create cm configmap-lua-scripts   --from-file=configmaps/lua-scripts -n ${N}

for i in cast-info movie movie-info plot review user
do
  oc create cm configmap-lua-scripts-${i} --from-file=configmaps/lua-scripts/wrk2-api/${i} -n ${N}
done

cd -
