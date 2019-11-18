#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

#tar -xzvf ${FILENAME}

mkdir out

# build lib
cd ${DIRNAME}
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=${CURDIR}/out
make
cd ..