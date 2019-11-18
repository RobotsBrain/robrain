#!/bin/bash



source param.mk

CURDIR=`pwd`

recreate_makefile_head()
{
	subdirs="liveMedia groupsock UsageEnvironment BasicUsageEnvironment testProgs mediaServer proxyServer"
	
	local newstr=$1
	local oldstr="/usr/local"

	for subdir in $subdirs
	do
	    sed -i "s#${oldstr}#${newstr}#g" $subdir/Makefile.head
	    sed -i "s#${oldstr}#${newstr}#g" $subdir/Makefile.tail
	done
}

if [ ! -f ${FILENAME} ]; then
	wget ${DOWNLOADURL}
fi

tar -xzvf ${FILENAME}

mkdir out

# build lib
cd ${DIRNAME}
recreate_makefile_head ${CURDIR}/out
./genMakefiles linux
make
make install
cd ..

cp -rf out/include/* ${CURDIR}/../prebuild/include/live555
cp out/lib/*.a ${CURDIR}/../prebuild/libs