#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -xzvf ${FILENAME}

# build lib
cd ${DIRNAME}
./genMakefiles linux
make
cd ..

#cp ${DIRNAME}/x264.h ${CURDIR}/../prebuild/include/x264
#cp ${DIRNAME}/libx264.a ${CURDIR}/../prebuild/libs