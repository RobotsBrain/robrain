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

cp -rf ${CURDIR}/out/include/* ${CURDIR}/../prebuild/include/pcap
cp ${CURDIR}/out/lib/libpcap.a ${CURDIR}/../prebuild/libs