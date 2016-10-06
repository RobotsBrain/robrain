#!/bin/bash

source param.mk

CURDIR=`pwd`

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

unzip ${FILENAME}

mkdir -p out/include
mkdir -p out/lib

# build lib
cd ${DIRNAME}
# dos2unix configure
#cat configure | tr -d '\r' > temp.sh
#mv temp.sh configure

FILES="configure ltconfig config.sub config.guess"

for file in ${FILES};
do
	sed -i 's/\r//' $file
	chmod +x $file
done

./configure --enable-shared --enable-static --host=linux --prefix=${CURDIR}/out

cp /usr/bin/libtool ./

make
make install-lib
make install-headers
cd ..

cp ${CURDIR}/out/include/*.h ${CURDIR}/../prebuild/include/jpeg
cp ${CURDIR}/out/lib/libjpeg.a ${CURDIR}/../prebuild/libs