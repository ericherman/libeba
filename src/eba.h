/*
eba.h: embedable bit array - hopefully somewhat suitable for small CPUs
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

#ifndef EBA_H
#define EBA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>		/* size_t */

/* embedable bit vector */
struct eba_s {
	unsigned char *bits;
	size_t size;
};

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba_s *eba, unsigned long index);

void eba_toggle(struct eba_s *eba, unsigned long index);

/**********************************************************************/
/* failure handling macros */
/**********************************************************************/
/* if no Eba_crash macro is defined, we use standard C lib */
#ifndef Eba_crash
#include <stdlib.h>

/* called in functions which return void */
#define Eba_crash() exit(EXIT_FAILURE)

/* called in functions which return unsigned char */
#define Eba_crash_uc() exit(EXIT_FAILURE)

#endif /* Eba_crash */

/**********************************************************************/
/* allocation convience functions */
/**********************************************************************/
#ifndef EBA_SKIP_EBA_NEW

/* #define Eba_alloc and Eba_free to use something other than malloc/free */

#ifndef Eba_alloc
#include <stdlib.h>
#define Eba_alloc malloc
#define Eba_free free
#endif

struct eba_s *eba_new(unsigned long num_bits);

void eba_free(struct eba_s *eba);
#endif /*EBA_SKIP_EBA_NEW */

/**********************************************************************/
/* logging macros and functions */
/**********************************************************************/
/* if no logging macros are defined, we use std C lib
   alternatively define each of:
   Eba_log_error0, Eba_log_error1, Eba_log_error2, Eba_log_error3
*/

#ifndef Eba_log_error0
#define EBA_NEED_GLOBAL_LOG_FILE
#include <stdio.h>		/* FILE */
extern FILE *eba_global_log_file;

#define Eba_log_file() \
 (eba_global_log_file == NULL) ? stderr : eba_global_log_file

#define Eba_log_error0(format) \
 fprintf(Eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(Eba_log_file(), format); \
 fprintf(Eba_log_file(), "\n")

#define Eba_log_error1(format, arg) \
 fprintf(Eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(Eba_log_file(), format, arg); \
 fprintf(Eba_log_file(), "\n")

#define Eba_log_error2(format, arg1, arg2) \
 fprintf(Eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(Eba_log_file(), format, arg1, arg2); \
 fprintf(Eba_log_file(), "\n")

#define Eba_log_error3(format, arg1, arg2, arg3) \
 fprintf(Eba_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(Eba_log_file(), format, arg1, arg2, arg3); \
 fprintf(Eba_log_file(), "\n")

#endif /* Eba_log_error0 */

#ifdef __cplusplus
}
#endif

#endif /* EBA_H */
