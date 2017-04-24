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

#ifdef EBA_SIZE_TYPE
typedef EBA_SIZE_TYPE size_t;
#else
#include <stddef.h>		/* size_t */
#endif

#ifndef EBA_SKIP_ENDIAN
enum eba_endian {
	eba_endian_little,
	eba_big_endian
};
#endif

/* embedable bit vector */
struct eba_s {
	unsigned char *bits;
	size_t size_bytes;
#ifndef EBA_SKIP_ENDIAN
	enum eba_endian endian;
#endif
};

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba_s *eba, unsigned long index);

void eba_toggle(struct eba_s *eba, unsigned long index);

void eba_swap(struct eba_s *eba, unsigned long index1, unsigned long index2);

#ifndef EBA_SKIP_SHIFTS
void eba_ring_shift_left(struct eba_s *eba, unsigned long positions);

void eba_ring_shift_right(struct eba_s *eba, unsigned long positions);

void eba_shift_left(struct eba_s *eba, unsigned long positions);

void eba_shift_right(struct eba_s *eba, unsigned long positions);

void eba_shift_left_fill(struct eba_s *eba, unsigned long positions,
			 unsigned char fillval);

void eba_shift_right_fill(struct eba_s *eba, unsigned long positions,
			  unsigned char fillval);
#endif

/**********************************************************************/
/* bits in a byte */
/**********************************************************************/
/*
 * C defines a char to be at least 8 bits,
 * but a char *may* be more ...
 * thus use CHAR_BIT from limits.h
 * unless compiled with -DEBA_CHAR_BIT=8 or similar
 */
#ifndef EBA_CHAR_BIT
#ifdef CHAR_BIT
#define EBA_CHAR_BIT CHAR_BIT
#else
#include <limits.h>
#ifdef CHAR_BIT
#define EBA_CHAR_BIT CHAR_BIT
#endif
#endif
#endif
#ifndef EBA_CHAR_BIT
#define EBA_CHAR_BIT 8
#endif

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

#ifndef EBA_SKIP_SHIFTS

/**********************************************************************/
/* memcpy */
/**********************************************************************/
/* if you wish, you can define your own memcpy
 * alternatively you may define EBA_DIY_MEMCPY to avoid standard C lib
 * the default is to simply use memcpy from the standard C lib */
#ifndef Eba_memcpy
#ifdef EBA_DIY_MEMCPY
#define EBA_NEED_DIY_MEMCPY
void *eba_diy_memcpy(void *dest, const void *src, size_t n);
#define Eba_memcpy eba_diy_memcpy
#else
#include <string.h>
#define Eba_memcpy memcpy
#endif
#endif /* Eba_memcpy */

/**********************************************************************/
/* internal (stack) allocation functions */
/**********************************************************************/
/*
 * In the shifting code, I would like to simly write something like:
 * "unsigned char bytes[foo];" where "foo" is a function parameter
 * But this is not widely supported.
 *
 * While C99,C11 have variable stack allocation, C++ and C89 do not.
 * As it happens, gcc supports alloca even for 8bit CPUs.
 * I would like wider-spread support of more modern C99/C11 especially
 * from Visual C++, but for now I'll just do this.
 *
 * This whole Eba_stack_alloc idea should be replaced with something
 * better, but I am out of ideas at the moment.
 */
#ifndef Eba_stack_alloc
#ifdef EBA_NO_ALLOCA
#include <stdlib.h>
#define Eba_stack_alloc malloc
#define Eba_stack_alloc_str "malloc"
#define EBA_NEED_DO_STACK_FREE
void eba_do_stack_free(void *ptr, size_t size);
#define Eba_stack_free eba_do_stack_free
#else
#include <alloca.h>
#define Eba_stack_alloc alloca
#define Eba_stack_alloc_str "alloca"
#define EBA_NEED_NO_STACK_FREE
void eba_no_stack_free(void *ptr, size_t size);
#define Eba_stack_free eba_no_stack_free
#endif
#endif /* Eba_stack_alloc */

#endif /* ifndef EBA_SKIP_SHIFTS */

/**********************************************************************/
/* allocation convience functions */
/**********************************************************************/
#ifndef EBA_SKIP_EBA_NEW

/* #define Eba_alloc and Eba_free to use something other than malloc/free */
/* #define Eba_alloc_str for using in error message log lines */

#ifndef Eba_alloc
#include <stdlib.h>
#define Eba_alloc malloc
#define Eba_alloc_str "malloc"
#define Eba_free free
#endif

#ifndef Eba_alloc_str
#define "(custom) allocate"
#endif

#ifndef EBA_SKIP_ENDIAN
struct eba_s *eba_new(unsigned long num_bits, enum eba_endian endian);
#else
struct eba_s *eba_new(unsigned long num_bits);
#endif

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
