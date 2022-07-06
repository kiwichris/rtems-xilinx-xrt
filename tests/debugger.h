/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Debugger support
 */

#include <stdbool.h>

#ifndef DEBUGGER_H
#define DEBUGGER_H

void debugger_init(bool cmd, bool start, bool wait);

#endif /* DEBUGGER_H */
