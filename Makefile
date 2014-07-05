TOP_DIR := $(shell pwd)

CC=gcc
CPP=g++
AR=ar

CFLAGS= -g -O2 -Wall
CFLAGS+= -I $(TOP_DIR)/include/x264 \
		-I $(TOP_DIR)/include/robrain

LDLIBS= -lpthread -lm

export TOP_DIR
export CC AR
export CFLAGS LDLIBS

SUBDIRS = src

all:
	for i in $(SUBDIRS); \
	do make -C $$i ; done

clean:
	for i in $(SUBDIRS); \
	do make -C $$i clean; done
