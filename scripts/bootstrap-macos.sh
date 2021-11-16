#!/usr/bin/env -i bash

HOME=/Users/jeff
DIR=$HOME/Code/strykeforce/deadeye/daemon
BUILD_DIR=$DIR/build
export PATH=/usr/local/bin:/bin:/usr/bin

rm -rf $BUILD_DIR
mkdir $BUILD_DIR

cmake -G Ninja -S $DIR -B $BUILD_DIR \
  -DDEADEYE_NT_SERVER=192.168.1.30 \
  -DDEADEYE_UNIT_ID=C


