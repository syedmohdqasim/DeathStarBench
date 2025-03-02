#!/bin/bash

NS=ai4cloudops-f7f10d9

cd $(dirname $0)/../..

oc create cm media-frontend-nginx --from-file=media-frontend-config/nginx.conf  --dry-run --save-config -o yaml -n ${NS} | oc apply -f - -n ${NS}
oc create cm media-frontend-lua   --from-file=media-frontend-config/lua-scripts --dry-run --save-config -o yaml -n ${NS} | oc apply -f - -n ${NS}
