# syntax=docker/dockerfile:1

FROM j3ff/deadeye-cpp-base:latest
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /build

COPY ./src/ /src/

RUN cmake \
    -D CMAKE_BUILD_TYPE=Release \
    /src \
    && make -j$(nproc)
