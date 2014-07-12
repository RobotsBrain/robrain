RB_LOG_TEST=rb_log_test
RB_VIDEO_TEST=rb_video_test
RB_DLIST_TEST=rb_dlist_test

CFLAGS += -I $(TOP_DIR)/src
LDFLAGS += -L $(TOP_DIR)/src -L $(TOP_DIR)/lib/.libs
RB_LDLIBS += -lrobrain -lx264

SRCS=rb_log_test.c
VIDEO_SRC=rb_video_test.c
RB_DLIST_SRC=rb_dlist_test.c

all: $(RB_LOG_TEST) $(RB_VIDEO_TEST) $(RB_DLIST_TEST)

$(RB_LOG_TEST): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(RB_LDLIBS) $(LDLIBS) -static

$(RB_VIDEO_TEST): $(VIDEO_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(RB_LDLIBS) $(LDLIBS) -static

$(RB_DLIST_TEST): $(RB_DLIST_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(RB_LDLIBS) $(LDLIBS) -static

clean:
	rm -f $(RB_LOG_TEST) $(RB_VIDEO_TEST) $(RB_DLIST_TEST) *.log *.h264

