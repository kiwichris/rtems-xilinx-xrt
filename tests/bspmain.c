/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include <bsp.h>
#include <bsp/bootcard.h>

/*
 * This routine calls main from a confdefs.h default Init task
 * set up. The bootcard will provide the task argument as
 * command line string (ASCIIZ).
 */

int main(int argc, char* argv[]);
void Init(rtems_task_argument arg);

static void console_init(void) {
  struct termios term;
  if (tcgetattr(fileno(stdout), &term) < 0) {
    printf("console: get terminal attributes: %s", strerror(errno));
  }
  cfsetispeed(&term, B115200);
  cfsetospeed(&term, B115200);
  if (tcsetattr(fileno(stdout), TCSADRAIN, &term) < 0) {
    printf("console: set terminal attributes: %s", strerror(errno));
  }
  if (tcgetattr(fileno(stdin), &term) < 0) {
    printf("console: get terminal attributes: %s", strerror(errno));
  }
  cfsetispeed(&term, B115200);
  cfsetospeed(&term, B115200);
  if (tcsetattr(fileno(stdin), TCSADRAIN, &term) < 0) {
    printf("console: terminal attributes: %s", strerror(errno));
  }
}


void Init(rtems_task_argument arg) {
  const char* boot_cmdline = *((const char**) arg);
  char*       cmdline = 0;
  char*       command;
  int         argc = 0;
  char**      argv = NULL;
  int         result = -124;
  if (boot_cmdline) {
    cmdline = malloc (strlen (boot_cmdline) + 1);
    if (cmdline) {
      strcpy(cmdline, boot_cmdline);
      command = cmdline;
      /*
       * Break the line up into arguments with "" being ignored.
       */
      while (true) {
        command = strtok(command, " \t\r\n");
        if (command == NULL) {
          break;
        }
        argc++;
        command = '\0';
      }
      argv = calloc(argc, sizeof (char*));
      if (argv) {
        int a;
        command = cmdline;
        argv[0] = command;
        for (a = 1; a < argc; a++) {
          command += strlen(command) + 1;
          argv[a] = command;
        }
      }
      else
        argc = 0;
    }
  }
  console_init();
  result = main(argc, argv);
  free(argv);
  free(cmdline);
  exit(result);
}
