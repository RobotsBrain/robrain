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
autoreconf -fiv
./configure --prefix=${CURDIR}/out --enable-static
make
make install
cd ..

cp -rf ${CURDIR}/out/include/mp4v2/* ${CURDIR}/../prebuild/include/mp4v2
cp ${CURDIR}/out/lib/libmp4v2.a ${CURDIR}/../prebuild/libs

