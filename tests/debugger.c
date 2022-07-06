/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Debugger support
 */

#include <errno.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#include <rtems/rtems-debugger.h>
#include <rtems/rtems-debugger-remote-tcp.h>

extern rtems_shell_cmd_t rtems_shell_DEBUGGER_Command;

static volatile int debugger_cont;

void debugger_init(bool cmd, bool start, bool wait) {
  int r;
  r = rtems_debugger_register_tcp_remote();
  if (r < 0) {
    printf("debugger: tcp remote register: %s", strerror(errno));
    return;
  }
  if (cmd) {
    rtems_shell_add_cmd(rtems_shell_DEBUGGER_Command.name,
                        rtems_shell_DEBUGGER_Command.topic,
                        rtems_shell_DEBUGGER_Command.usage,
                        rtems_shell_DEBUGGER_Command.command);
  }
  if (start) {
    rtems_printer printer;
    rtems_print_printer_fprintf(&printer, stdout);
    int r = rtems_debugger_start(
      "tcp", "1122", RTEMS_DEBUGGER_TIMEOUT, 1, &printer);
    if (r < 0) {
      printf("error: debugger: failed to start\n");
    }
    sleep(1);
  }
  if (wait) {
    size_t count = 0;
    printf("debugger: waiting ...   ");
    while (debugger_cont == 0) {
      const char wiggle[4] = { '|', '/', '-', '\\' };
      printf("\b\b%c ", wiggle[count]);
      fflush(stdout);
      ++count;
      if (count >= sizeof(wiggle)) {
        count = 0;
      }
      sleep(1);
    }
  }
}
