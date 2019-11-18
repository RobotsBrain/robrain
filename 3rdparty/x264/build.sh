#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -jxvf ${FILENAME}

# build lib
cd ${DIRNAME}
./configure --disable-opencl
make
cd ..

cp ${DIRNAME}/x264.h ${CURDIR}/../prebuild/include/x264
cp ${DIRNAME}/libx264.a ${CURDIR}/../prebuild/libs