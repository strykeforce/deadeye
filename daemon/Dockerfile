# syntax=docker/dockerfile:1

FROM j3ff/deadeye-cpp-base:latest
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /opt/build

COPY . /opt/deadeye
WORKDIR /opt/deadeye/build

RUN cmake \
    -D CMAKE_BUILD_TYPE=Debug \
    -D DEADEYE_BUILD_TESTS=ON \
    -D DEADEYE_UNIT_ID=D \
    -D DEADEYE_NT_SERVER=10.27.67.2 \
    -D DEADEYE_CAMERA0_PIPELINE=deadeye::UprightRectPipeline \
    -D DEADEYE_CAMERA1_PIPELINE=deadeye::MinAreaRectPipeline \
    -D DEADEYE_CAMERA2_PIPELINE=deadeye::TargetListPipeline \
    /opt/deadeye \
    && make -j$(nproc) \
    && mkdir /opt/images

VOLUME [ "/opt/images" ]
EXPOSE 5805/tcp

CMD ["./src/deadeyed"]
