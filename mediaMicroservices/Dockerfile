FROM smqasim/thrift-microservice-deps:xenial-media
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        openssl \
        ca-certificates \
        libsasl2-2 \
        libmemcached11 \
        libmemcachedutil2 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN apt-get update
RUN apt-get -y install libcurl4-openssl-dev

COPY ./ /media-microservices
RUN cd /media-microservices \
    && mkdir -p build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Debug .. \
    && make -j$(nproc) \
    && make install

WORKDIR /media-microservices
