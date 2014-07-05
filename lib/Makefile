SUBDIRS = x264

LIBDIR=$(TOP_DIR)/lib/.libs

all:
	for i in $(SUBDIRS); \
	do cp Makefile.$$i $$i/makefile; done
	for i in $(SUBDIRS); \
	do make -C $$i -f makefile; done

clean:
	for i in $(SUBDIRS); \
	do	make -C $$i clean -f makefile; done
	for i in $(SUBDIRS); \
	do rm -f $$i/makefile done
