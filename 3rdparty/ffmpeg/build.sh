#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -jxvf ${FILENAME}

mkdir out

# build lib
cd ${DIRNAME}
./configure  --enable-shared --disable-yasm --prefix=${CURDIR}/out
make
make install
cd ..

#cp ${DIRNAME}/x264.h ${CURDIR}/../prebuild/include/x264
#cp ${DIRNAME}/libx264.a ${CURDIR}/../prebuild/libs