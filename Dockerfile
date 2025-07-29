FROM ubuntu:plucky

SHELL ["/bin/bash", "-c"]
ENV TZ=UTC DEBIAN_FRONTEND=noninteractive

RUN \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        llvm clang lld libomp-dev cmake ninja-build pkgconf \
        libhts-dev libdivsufsort-dev libomp5 libhts3 libdivsufsort3 \
        samtools gdb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
ENV CC=clang CXX=clang++

WORKDIR /app/integrate
#DEBUGGING PURPOSE DO NOT COPY: 
#COPY . .

# DEBUGGING PURPOSE Do NOT copy code or build it here! 
# We will copy source & build at runtime in the cluster, so comment the RUN and WORKDIR
# RUN rm -rf build && \
#    mkdir build && \
#    cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local && \
#    ninja -C build install

# WORKDIR /app/integrate

CMD ["/bin/bash"]