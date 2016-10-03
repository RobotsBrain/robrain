#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

unzip ${FILENAME}

# build lib
cd ${DIRNAME}
# dos2unix configure
# sed -i 's/\r//' filename
cat configure | tr -d '\r' > temp.sh
mv temp.sh configure
chmod +x configure

./configure
make
cd ..

cp ${DIRNAME}/jpegint.h ${CURDIR}/../prebuild/include/jpeg
cp ${DIRNAME}/jerror.h ${CURDIR}/../prebuild/include/jpeg
cp ${DIRNAME}/jconfig.h ${CURDIR}/../prebuild/include/jpeg
cp ${DIRNAME}/jmorecfg.h ${CURDIR}/../prebuild/include/jpeg
cp ${DIRNAME}/jpeglib.h ${CURDIR}/../prebuild/include/jpeg
cp ${DIRNAME}/libjpeg.a ${CURDIR}/../prebuild/libs