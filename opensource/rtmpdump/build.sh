#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -d ${DIRNAME} ]; then
	git clone ${DOWNLOADURL}
fi

# build lib
cd ${DIRNAME}
make
cd ..

cp ${CURDIR}/rtmpdump/librtmp/rtmp.h ${CURDIR}/../prebuild/include/rtmpdump
cp ${CURDIR}/rtmpdump/librtmp/librtmp.a ${CURDIR}/../prebuild/libs