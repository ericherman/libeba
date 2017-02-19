/*
eba-log.h: definitions for error reporting
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

#ifndef EBA_LOG_H
#define EBA_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* if no Eba_crash macro is defined, we use standard C lib */
#ifndef Eba_crash
#include <stdlib.h>
#define Eba_crash() exit(EXIT_FAILURE)
#define Eba_crash_uc() exit(EXIT_FAILURE)
#endif

/* if no logging macros are defined, we use std C lib */
/* and the impl from eba-log.c may be used, as well */
#ifndef Eba_log_error0
#include <stdio.h>		/* FILE */

/* Get the FILE pointer to where fprintf messages currently target.
   Defaults to stderr. */
FILE *eba_log_file();

/* Set the FILE pointer to where fprintf messages shall target. */
void set_eba_log_file(FILE *log);

/* if _POSIX_C_SOURCE backtrace_symbols_fd is used */
void eba_log_backtrace(FILE *log);

#define Eba_log_error0(format) \
 fprintf(eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(eba_log_file(), format); \
 fprintf(eba_log_file(), "\n"); \
 eba_log_backtrace(eba_log_file())
#endif

#ifndef Eba_log_error1
#define Eba_log_error1(format, arg) \
 fprintf(eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(eba_log_file(), format, arg); \
 fprintf(eba_log_file(), "\n"); \
 eba_log_backtrace(eba_log_file())
#endif

#ifndef Eba_log_error2
#define Eba_log_error2(format, arg1, arg2) \
 fprintf(eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(eba_log_file(), format, arg1, arg2); \
 fprintf(eba_log_file(), "\n"); \
 eba_log_backtrace(eba_log_file())
#endif

#ifndef Eba_log_error3
#define Eba_log_error3(format, arg1, arg2, arg3) \
 fprintf(eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(eba_log_file(), format, arg1, arg2, arg3); \
 fprintf(eba_log_file(), "\n"); \
 eba_log_backtrace(eba_log_file())
#endif

#ifdef __cplusplus
}
#endif

#endif /* EBA_LOG_H */
