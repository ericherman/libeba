/*
eba-log.c: logging info
Copyright (C) 2017 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/

#include "eba-log.h"

#ifdef _POSIX_SOURCE
#include <execinfo.h>		/* backtrace backtrace_symbols_fd */
#include <stdio.h>		/* fileno */
#endif

FILE *global_eba_log_file = NULL;

FILE *eba_log_file()
{
	if (global_eba_log_file == NULL) {
		global_eba_log_file = stderr;
	}
	return global_eba_log_file;
}

void set_eba_log_file(FILE *log)
{
	global_eba_log_file = log;
}

void eba_log_backtrace(FILE *log)
{
#if _POSIX_SOURCE
	void *array[4096];
	size_t size;

	size = backtrace(array, 4096);
	backtrace_symbols_fd(array, size, fileno(log));
#else
	fprintf(log, "(backtrace unavailable)\n");
#endif /* EHBI_CAN_BACKTRACE */
}
