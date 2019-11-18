#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -xzvf ${FILENAME}

mkdir out

# build lib
cd ${DIRNAME}
./configure --prefix=${CURDIR}/out
make
make install
cd ..

cp ${CURDIR}/include/speex/*.h ${CURDIR}/../prebuild/include/speex
cp ${CURDIR}/lib/libspeex.a ${CURDIR}/../prebuild/libs