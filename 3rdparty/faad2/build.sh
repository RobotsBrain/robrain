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

cp -rf ${CURDIR}/out/include/*.h ${CURDIR}/../prebuild/include/faad2
cp ${CURDIR}/out/lib/libfaad.a ${CURDIR}/../prebuild/libs
