FROM smqasim/thrift-microservice-deps:xenial-media

COPY ./ /media-microservices
RUN cd /media-microservices \
    && mkdir -p build \
    && cd build \
    && cmake .. \
    && make \
    && make install

WORKDIR /media-microservices
