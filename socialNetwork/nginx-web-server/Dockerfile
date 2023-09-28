FROM yg397/openresty-thrift:xenial
COPY lua-scripts /usr/local/openresty/nginx/lua-scripts
COPY pages /usr/local/openresty/nginx/pages
COPY conf/nginx.conf /usr/local/openresty/nginx/conf/nginx.conf
COPY gen-lua /gen-lua
COPY ../docker/openresty-thrift/lua-thrift /usr/local/openresty/lualib/thrift