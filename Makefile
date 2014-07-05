TOP_DIR := $(shell pwd)

CC=gcc
CPP=g++
AR=ar

export TOP_DIR
export CC AR

SUBDIRS = src

all:
	for i in $(SUBDIRS); \
	do make -C $$i ; done

clean:
	for i in $(SUBDIRS); \
	do make -C $$i clean; done
