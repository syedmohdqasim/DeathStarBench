FROM yg397/openresty-thrift:xenial
COPY lua-scripts /usr/local/openresty/nginx/lua-scripts
COPY pages /usr/local/openresty/nginx/pages
COPY gen-lua /gen-lua
COPY lua-thrift /usr/local/openresty/lualib/thrift
RUN chmod -R 777 /usr/local/openresty/nginx/