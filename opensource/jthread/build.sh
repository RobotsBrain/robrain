#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -jxvf ${FILENAME}

# build lib
cd ${DIRNAME}
cmake ./ -DCMAKE_INSTALL_PREFIX=${CURDIR}
make
make install
cd ..

cp ${CURDIR}/include/jthread/*.h ${CURDIR}/../prebuild/include/jthread
cp ${CURDIR}/lib/libjthread.a ${CURDIR}/../prebuild/libs