#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

unzip ${FILENAME}

mkdir include
mkdir lib

# build lib
cd ${DIRNAME}
# dos2unix configure
# sed -i 's/\r//' filename
cat configure | tr -d '\r' > temp.sh
mv temp.sh configure
chmod +x configure

./configure --enable-shared --enable-static --prefix=${CURDIR}
make
make install-lib
make install-headers
cd ..

cp ${CURDIR}/*.h ${CURDIR}/../prebuild/include/jpeg
cp ${CURDIR}/libjpeg.a ${CURDIR}/../prebuild/libs