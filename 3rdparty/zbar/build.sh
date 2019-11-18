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
./configure --disable-video --without-imagemagick --without-gtk --without-python --without-qt --prefix=${CURDIR}/out
make
make install
cd ..

cp -rf ${CURDIR}/out/include/* ${CURDIR}/../prebuild/include/zbar
cp ${CURDIR}/out/lib/libzbar.a ${CURDIR}/../prebuild/libs