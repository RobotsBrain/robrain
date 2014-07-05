LIBROBRAIN=librobrain.a
LIBROBRAIN_SHARED=librobrain.so
LIBROBRAIN_VERS=0.1

CFLAGS += -I ./
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

SUBDIRS = tests

all: libs test

libs: $(OBJS)
	$(AR) -rc $(LIBROBRAIN) $^
	$(CC) -shared $(CFLAGS) -o $(LIBROBRAIN_SHARED) $^

test:
	for i in $(SUBDIRS); \
	do make -C $$i ; done

clean:
	rm -f *.[oa] *.so
	for i in $(SUBDIRS); \
	do make -C $$i clean; done
