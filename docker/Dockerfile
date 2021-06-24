# syntax=docker/dockerfile:1

FROM ubuntu:18.04
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /opt/build

ARG OPENCV_VERSION="4.5.2"
ARG WPILIB_VERSION="2021.3.1"
ARG FMT_VERSION="7.1.3"
ARG SPDLOG_VERSION="1.8.5"
ARG NLOHMANN_JSON_VERSION="3.9.1"
ARG SAFE_VERSION="1.0.1"
ARG READERWRITERQUEUE_VERSION="1.0.5"
ARG TINYFSM_VERSION="0.3.3"
ARG CATCH2_VERSION="2.13.6"
ARG BUILD_LOG=/opt/build.log

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    gstreamer1.0-gl \
    gstreamer1.0-libav \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-tools \
    gstreamer1.0-x \
    libavformat-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer1.0-0 \
    libjpeg-dev \
    libopenjp2-7-dev \
    libpng-dev \
    libswscale-dev \
    libsystemd-dev \
    libtbb-dev \
    libtbb2 \
    pkg-config \
    unzip \
    wget \
    yasm \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O opencv.zip \
    && unzip opencv.zip -d /opt \
    && rm -rf opencv.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D BUILD_JPEG=ON \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_PNG=ON \
    -D BUILD_TESTS=OFF \
    -D BUILD_TIFF=OFF \
    -D BUILD_opencv_java=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_text=ON \
    -D WITH_CUDA=OFF \
    -D WITH_EIGEN=ON \
    -D WITH_GPHOTO2=OFF \
    -D WITH_GSTREAMER=ON \
    -D WITH_GSTREAMER_0_10=OFF \
    -D WITH_IPP=ON \
    -D WITH_OPENCL=ON \
    -D WITH_OPENGL=ON \
    -D WITH_QT=OFF \
    -D WITH_TBB=ON \
    -D WITH_V4L=ON \
    /opt/opencv-${OPENCV_VERSION} \
    | tee -a ${BUILD_LOG} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/opencv-${OPENCV_VERSION} 

RUN wget https://github.com/wpilibsuite/allwpilib/archive/v${WPILIB_VERSION}.zip -O wpilib.zip \
    && unzip wpilib.zip -d /opt \
    && rm -rf wpilib.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D FLAT_INSTALL_WPILIB=ON \
    -D WITH_CSCORE=ON \
    -D WITH_EXAMPLES=OFF \
    -D WITH_GUI=OFF \
    -D WITH_JAVA=OFF \
    -D WITH_OLD_COMMANDS=OFF \
    -D WITH_SIMULATION_MODULES=OFF \
    -D WITH_TESTS=OFF \
    -D WITH_WPILIB=OFF \
    -D WITH_WPIMATH=OFF \
    /opt/allwpilib-${WPILIB_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/allwpilib-${WPILIB_VERSION}

RUN wget  https://github.com/fmtlib/fmt/archive/${FMT_VERSION}.zip -O fmt.zip \
    && unzip fmt.zip -d /opt \
    && rm -rf fmt.zip \
    && cmake \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D FMT_DOC=OFF \
    -D FMT_TEST=OFF \
    /opt/fmt-${FMT_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/fmt-${FMT_VERSION}

RUN wget  https://github.com/gabime/spdlog/archive/v${SPDLOG_VERSION}.zip -O spdlog.zip \
    && unzip spdlog.zip -d /opt \
    && rm -rf spdlog.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D SPDLOG_FMT_EXTERNAL=ON \
    /opt/spdlog-${SPDLOG_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/spdlog-${SPDLOG_VERSION}

RUN wget  https://github.com/nlohmann/json/archive/v${NLOHMANN_JSON_VERSION}.zip -O json.zip \
    && unzip json.zip -d /opt \
    && rm -rf json.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D JSON_BuildTests=OFF \
    /opt/json-${NLOHMANN_JSON_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/json-${NLOHMANN_JSON_VERSION}

RUN wget  https://github.com/LouisCharlesC/safe/archive/v${SAFE_VERSION}.zip -O safe.zip \
    && unzip safe.zip -d /opt \
    && rm -rf safe.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/safe-${SAFE_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/safe-${SAFE_VERSION}

RUN wget  https://github.com/cameron314/readerwriterqueue/archive/v${READERWRITERQUEUE_VERSION}.zip -O readerwriterqueue.zip \
    && unzip readerwriterqueue.zip -d /opt \
    && rm -rf readerwriterqueue.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/readerwriterqueue-${READERWRITERQUEUE_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/readerwriterqueue-${READERWRITERQUEUE_VERSION}

RUN wget  https://github.com/jhh/tinyfsm/archive/v${TINYFSM_VERSION}.zip -O tinyfsm.zip \
    && unzip tinyfsm.zip -d /opt \
    && rm -rf tinyfsm.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/tinyfsm-${TINYFSM_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/tinyfsm-${TINYFSM_VERSION}


RUN wget  https://github.com/catchorg/Catch2/archive/v${CATCH2_VERSION}.zip -O catch.zip \
    && unzip catch.zip -d /opt \
    && rm -rf catch.zip \
    && cmake \
    -D CATCH_BUILD_TESTING=OFF \
    -D CATCH_INSTALL_DOCS=OFF \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/Catch2-${CATCH2_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/Catch2-${CATCH2_VERSION} 
