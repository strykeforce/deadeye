# syntax=docker/dockerfile:1

# hadolint ignore=DL3007
FROM j3ff/deadeye-cpp-base:latest AS base
LABEL maintainer=“jeff@j3ff.io”

# build environment
FROM base as build
WORKDIR /build

COPY . /src/

RUN cmake \
    -D CMAKE_BUILD_TYPE=Debug \
    -D DEADEYE_BUILD_TESTS=ON \
    -D DEADEYE_UNIT_ID=D \
    -D DEADEYE_NT_SERVER=10.27.67.2 \
    -D DEADEYE_STREAM_ADDRESS=10.27.67.11 \
    -D DEADEYE_CAMERA0_PIPELINE=deadeye::TargetListPipeline \
    -D DEADEYE_CAMERA1_PIPELINE=deadeye::UprightRectPipeline \
    /src \
    && make "-j$(nproc)"

# production environment
FROM base
WORKDIR /app
COPY --from=build /build/src/deadeyed /app/
VOLUME [ "/opt/images" ]
EXPOSE 5805/tcp
EXPOSE 5806/tcp
EXPOSE 5807/tcp
EXPOSE 5808/tcp
EXPOSE 5809/tcp

CMD ["./deadeyed"]
