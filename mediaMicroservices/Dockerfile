FROM smqasim/thrift-microservice-deps:xenial-media
RUN apt-get -y install libcurl4-openssl-dev
COPY ./ /media-microservices
RUN cd /media-microservices \
    && mkdir -p build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Debug .. \
    && make -j$(nproc) \
    && make install

WORKDIR /media-microservices
