#!/bin/sh


BUILDDIR=output

if [ -d ${BUILDDIR} ]; then
	rm -rf ${BUILDDIR}
fi

mkdir ${BUILDDIR}
cd ${BUILDDIR}
cmake ../ -DTOOLCHAIN=$1
make
