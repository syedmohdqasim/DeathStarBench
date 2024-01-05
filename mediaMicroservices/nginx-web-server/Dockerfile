FROM yg397/openresty-thrift:xenial
COPY lua-scripts /usr/local/openresty/nginx/lua-scripts
COPY conf/nginx.conf:/usr/local/openresty/nginx/conf/nginx.conf
COPY gen-lua /gen-lua
COPY jaeger-config.json /usr/local/openresty/nginx/jaeger-config.json
RUN chmod -R 777 /usr/local/openresty/nginx/