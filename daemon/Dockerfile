# syntax=docker/dockerfile:1

FROM ubuntu:18.04
LABEL maintainer=“jeff@j3ff.io”

WORKDIR /opt/build

ENV OPENCV_VERSION="4.5.2"
ENV WPILIB_VERSION="2021.3.1"
ENV FMT_VERSION="7.1.3"
ENV SPDLOG_VERSION="1.8.2"
ENV NLOHMANN_JSON_VERSION="3.9.1"
ENV SAFE_VERSION="1.0.1"
ENV READERWRITERQUEUE_VERSION="1.0.3"
ENV TINYFSM_VERSION="0.3.3"
ENV CATCH2_VERSION="2.12.4"

RUN apt-get -qq update \
    && apt-get -qq install -y --no-install-recommends \
    build-essential \
    cmake \
    wget \
    ca-certificates \
    unzip \
    yasm \
    pkg-config \
    libswscale-dev \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libopenjp2-7-dev \
    libavformat-dev \
    libgstreamer1.0-0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-tools \
    libsystemd-dev

RUN wget -q https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O opencv.zip \
    && unzip -qq opencv.zip -d /opt \
    && rm -rf opencv.zip \
    && cmake \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D BUILD_opencv_java=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_text=ON \
    -D BUILD_TIFF=OFF \
    -D BUILD_JPEG=ON \
    -D BUILD_PNG=ON \
    -D WITH_CUDA=OFF \
    -D WITH_OPENGL=ON \
    -D WITH_OPENCL=ON \
    -D WITH_IPP=ON \
    -D WITH_TBB=ON \
    -D WITH_EIGEN=ON \
    -D WITH_V4L=ON \
    -D WITH_QT=OFF \
    -D WITH_GPHOTO2=OFF \
    -D WITH_GSTREAMER=ON \
    -D WITH_GSTREAMER_0_10=OFF \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D CMAKE_BUILD_TYPE=Release \
    /opt/opencv-${OPENCV_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/opencv-${OPENCV_VERSION} \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get -qq autoremove \
    && apt-get -qq clean

RUN wget -q https://github.com/wpilibsuite/allwpilib/archive/v${WPILIB_VERSION}.zip -O wpilib.zip \
    && unzip -qq wpilib.zip -d /opt \
    && rm -rf wpilib.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D WITH_JAVA=OFF \
    -D WITH_CSCORE=ON \
    -D WITH_WPIMATH=OFF \
    -D WITH_WPILIB=OFF \
    -D WITH_OLD_COMMANDS=OFF \
    -D WITH_EXAMPLES=OFF \
    -D WITH_TESTS=OFF \
    -D WITH_GUI=OFF \
    -D WITH_SIMULATION_MODULES=OFF \
    -D FLAT_INSTALL_WPILIB=ON \
    /opt/allwpilib-${WPILIB_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/allwpilib-${WPILIB_VERSION}


RUN wget -q  https://github.com/fmtlib/fmt/archive/${FMT_VERSION}.zip -O fmt.zip \
    && unzip -qq fmt.zip -d /opt \
    && rm -rf fmt.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D BUILD_SHARED_LIBS=ON \
    -D FMT_DOC=OFF \
    -D FMT_TEST=OFF \
    /opt/fmt-${FMT_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/fmt-${FMT_VERSION}



RUN wget -q  https://github.com/gabime/spdlog/archive/v${SPDLOG_VERSION}.zip -O spdlog.zip \
    && unzip -qq spdlog.zip -d /opt \
    && rm -rf spdlog.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D -DSPDLOG_FMT_EXTERNAL=ON \
    /opt/spdlog-${SPDLOG_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/spdlog-${SPDLOG_VERSION}



RUN wget -q  https://github.com/nlohmann/json/archive/v${NLOHMANN_JSON_VERSION}.zip -O json.zip \
    && unzip -qq json.zip -d /opt \
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



RUN wget -q  https://github.com/LouisCharlesC/safe/archive/v${SAFE_VERSION}.zip -O safe.zip \
    && unzip -qq safe.zip -d /opt \
    && rm -rf safe.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/safe-${SAFE_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/safe-${SAFE_VERSION}



RUN wget -q  https://github.com/cameron314/readerwriterqueue/archive/v${READERWRITERQUEUE_VERSION}.zip -O readerwriterqueue.zip \
    && unzip -qq readerwriterqueue.zip -d /opt \
    && rm -rf readerwriterqueue.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/readerwriterqueue-${READERWRITERQUEUE_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/readerwriterqueue-${READERWRITERQUEUE_VERSION}



RUN wget -q  https://github.com/jhh/tinyfsm/archive/v${TINYFSM_VERSION}.zip -O tinyfsm.zip \
    && unzip -qq tinyfsm.zip -d /opt \
    && rm -rf tinyfsm.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    /opt/tinyfsm-${TINYFSM_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/tinyfsm-${TINYFSM_VERSION}


RUN wget -q  https://github.com/catchorg/Catch2/archive/v${CATCH2_VERSION}.zip -O catch.zip \
    && unzip -qq catch.zip -d /opt \
    && rm -rf catch.zip \
    && cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D CATCH_BUILD_TESTING=OFF \
    -D CATCH_INSTALL_DOCS=OFF \
    /opt/Catch2-${CATCH2_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/Catch2-${CATCH2_VERSION}

COPY . /opt/deadeye
WORKDIR /opt/deadeye/build

RUN cmake \
    -D CMAKE_BUILD_TYPE=Debug \
    -D DEADEYE_BUILD_TESTS=ON \
    -D DEADEYE_UNIT_ID=E \
    -D DEADEYE_NT_SERVER=10.27.67.2 \
    -D DEADEYE_CAMERA0_PIPELINE=deadeye::UprightRectPipeline \
    /opt/deadeye \
    && make -j$(nproc)

RUN mkdir /opt/images
VOLUME [ "/opt/images" ]
EXPOSE 5805/tcp

CMD ["./src/deadeyed"]
