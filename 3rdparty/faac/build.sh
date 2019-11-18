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
sed -i "126d" common/mp4v2/mpeg4ip.h
make
make install
cd ..

cp -rf ${CURDIR}/out/include/*.h ${CURDIR}/../prebuild/include/faac
cp ${CURDIR}/out/lib/libfaac.a ${CURDIR}/../prebuild/libs