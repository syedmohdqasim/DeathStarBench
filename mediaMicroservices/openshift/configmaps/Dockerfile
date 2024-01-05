FROM yg397/openresty-thrift:xenial
COPY lua-scripts /usr/local/openresty/nginx/lua-scripts
COPY pages /usr/local/openresty/nginx/pages
COPY nginx.conf /usr/local/openresty/nginx/conf/nginx.conf
COPY gen-lua /gen-lua
RUN chmod -R 777 /usr/local/openresty/nginx/