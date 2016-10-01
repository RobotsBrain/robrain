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

cp ${CURDIR}/include/jrtplib3/*.h ${CURDIR}/../prebuild/include/jrtplib
cp ${CURDIR}/lib/libjrtp.a ${CURDIR}/../prebuild/libs