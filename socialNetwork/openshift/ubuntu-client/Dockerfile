FROM ubuntu:latest
RUN apt-get -y update
RUN apt-get -y upgrade
RUN apt-get -y install dnsutils git vim python3 python3-aiohttp
RUN apt-get -y install libssl-dev libz-dev luarocks
RUN apt-get -y install iputils-ping lynx build-essential gcc bash curl luarocks
RUN useradd -m john
RUN mkdir /app
WORKDIR /app
RUN git clone https://github.com/syedmohdqasim/DeathStarBench.git
#WORKDIR  /app/DeathStarBench/socialNetwork/openshift/wrk2
#RUN ls
RUN mkdir -p /astrea
RUN chmod -R 777 /astrea
USER john
CMD /bin/bash