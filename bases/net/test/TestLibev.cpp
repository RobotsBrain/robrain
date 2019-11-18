#include <stdio.h>
#include <ev.h>

#include "base/Log.h"



ev_io stdin_watcher;
ev_timer timeout_watcher;

static void stdin_cb (EV_P_ ev_io *w, int revents)
{
  DEBUG("stdin ready\n");

  ev_io_stop(EV_A_ w);

  // ev_break(EV_A_ EVBREAK_ALL);

  return;
}

static void timeout_cb (EV_P_ ev_timer *w, int revents)
{
  DEBUG("timeout\n");

  ev_timer_stop(EV_A_ w);

  // ev_break(EV_A_ EVBREAK_ONE);

  return;
}

int main(int argc, char **argv)
{
  struct ev_loop *loop = EV_DEFAULT;

  INFO("major: %d, minor: %d\n", ev_version_major(), ev_version_minor());

  ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
  ev_io_start(loop, &stdin_watcher);

  ev_timer_init(&timeout_watcher, timeout_cb, 5.5, 2);
  ev_timer_start(loop, &timeout_watcher);

  ev_run(loop, 0);

  return 0;
}


