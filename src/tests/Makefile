RB_LOG_TEST=rb_log_test

CFLAGS += -I $(TOP_DIR)/src
LDFLAGS += -L $(TOP_DIR)/src
LDLIBS += -lrobrain

SRCS=rb_log_test.c

$(RB_LOG_TEST): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -static

clean:
	rm -f $(RB_LOG_TEST)
