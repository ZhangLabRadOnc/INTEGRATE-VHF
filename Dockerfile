FROM ubuntu:mantic AS builder

SHELL ["/bin/bash", "-c"]
ENV TZ=UTC DEBIAN_FRONTEND=noninteractive

RUN \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        llvm clang lld libomp-dev cmake ninja-build pkgconf \
        libhts-dev libdivsufsort-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV CC=clang CXX=clang++

WORKDIR /app/integrate
COPY . .
RUN \
    rm -rf /app/integrate/build && \
    mkdir ./build && \
    cmake -G Ninja -S ./ -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local && \
    ninja -C ./build install

FROM ubuntu:mantic

SHELL ["/bin/bash", "-c"]
ENV TZ=UTC DEBIAN_FRONTEND=noninteractive

RUN \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        libomp5 libhts3 libdivsufsort3 samtools && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local /usr/local

WORKDIR /data
