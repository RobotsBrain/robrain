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

cp ${CURDIR}/out/include/*.h ${CURDIR}/../prebuild/include/adns
cp ${CURDIR}/out/lib/libadns.a ${CURDIR}/../prebuild/libs