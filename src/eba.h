/*
eba.h: embedable bit array - hopefully somewhat suitable for small CPUs
Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>

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
#define EBA_H 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EBA_SKIP_ENDIAN
#define EBA_SKIP_ENDIAN 0
#endif

#define Eba_need_endian (!(EBA_SKIP_ENDIAN))

#ifndef EBA_SKIP_SET_ALL
#define EBA_SKIP_SET_ALL 0
#endif

#define Eba_need_set_all (!(EBA_SKIP_SET_ALL))

#ifndef EBA_SKIP_SHIFTS
#define EBA_SKIP_SHIFTS 0
#endif

#define Eba_need_shifts (!(EBA_SKIP_SHIFTS))

#ifndef EBA_SKIP_NEW
#define EBA_SKIP_NEW 0
#endif

#define Eba_need_new (!(EBA_SKIP_NEW))

#ifndef EBA_SKIP_STRUCT_NULL_CHECK
#define EBA_SKIP_STRUCT_NULL_CHECK 0
#endif

#define EBA_need_struct_null_check (!(EBA_SKIP_STRUCT_NULL_CHECK)

#ifndef EBA_SKIP_STRUCT_BITS_NULL_CHECK
#define EBA_SKIP_STRUCT_BITS_NULL_CHECK 0
#endif

#define Eba_need_struct_bits_null_check (!(EBA_SKIP_STRUCT_BITS_NULL_CHECK))

#ifndef EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY
#define EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY 0
#endif

#define Eba_need_array_index_overrun_safety \
	(!(EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY))

/**********************************************************************/
/* Standard C library available? */
/**********************************************************************/
/*
 * When compiling to embedded environments, __STDC_HOSTED__ can be
 * defined to be 1, however due to the increased size of the firmware
 * it may be desirable to _not_ use anything from the libc.
 *
 * If Eba_use_libc is defined to be zero, then hopefully using EBA
 * will not result in pulling in a bunch of bloat from libc.
 */
#ifndef EBA_SKIP_LIBC
#ifdef __STDC_HOSTED__
#define EBA_SKIP_LIBC !(__STDC_HOSTED__)
#else
/* guess that we are hosted */
#define EBA_SKIP_LIBC 1
#endif /*  __STDC_HOSTED__ */
#endif /* EBA_SKIP_LIBC */

#define Eba_use_libc (!(EBA_SKIP_LIBC))

/**********************************************************************/

#ifdef EBA_SIZE_TYPE
typedef EBA_SIZE_TYPE size_t;
#else
#include <stddef.h>		/* size_t */
#endif

/**********************************************************************/

/**********************************************************************/
/* bits in a byte */
/**********************************************************************/
/*
 * C defines a char to be at least 8 bits,
 * but a char *may* be more ...
 * thus use CHAR_BIT from limits.h
 * unless compiled with -DEBA_CHAR_BIT=8 or similar
 * POSIX insists that CHAR_BIT is 8, and all modern OSes this is true
 * of ... however the embedded space is not 100% uniform. Searching
 * for "#define CHARBIT 16" and "CHARBIT 32" has a few hits ....
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

#if Eba_need_endian
enum eba_endian {
	eba_endian_little,
	eba_big_endian
};
#endif

/* embedable bit vector */
struct eba_s {
	unsigned char *bits;
	size_t size_bytes;
#if Eba_need_endian
	enum eba_endian endian;
#endif
};

char *eba_to_string(struct eba_s *eba, char *buf, size_t len);

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba_s *eba, unsigned long index);

#if Eba_need_set_all
void eba_set_all(struct eba_s *eba, unsigned char val);
#endif

void eba_toggle(struct eba_s *eba, unsigned long index);

void eba_swap(struct eba_s *eba, unsigned long index1, unsigned long index2);

#if Eba_need_shifts
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
/* failure handling macros */
/**********************************************************************/
/* if no Eba_crash macro is defined, we use standard C lib */
#ifndef Eba_crash

/* called in functions which return void */
#if Eba_use_libc
#include <stdlib.h>
#define Eba_crash() exit(EXIT_FAILURE)

/* called in functions which return unsigned char */
#define Eba_crash_uc() exit(EXIT_FAILURE)
#else /* Eba_use_libc */
/* Cast NULL to an int-pointer, then write to it: BOOM! (we hope) */
#define Eba_crash()	do { *((int *)0)= 1; } while (0)
#define Eba_crash_uc()	do { *((int *)0)= 1; return 1; } while (0)
#endif /* Eba_use_libc */

#endif /* Eba_crash */

/**********************************************************************/
/* memset */
/**********************************************************************/
/* if you wish, you can define your own memset
 * alternatively you may define EBA_DIY_MEMSET to avoid standard C lib
 * the default is to simply use memset from the standard C lib */
#ifndef EBA_DIY_MEMSET
#define EBA_DIY_MEMSET 0
#endif

#ifndef Eba_memset
#if (EBA_DIY_MEMSET || EBA_SKIP_LIBC)
#define Eba_need_diy_memset 1
#define Eba_memset eba_diy_memset
#else
#include <string.h>
#define Eba_need_diy_memset 0
#define Eba_memset memset
#endif
#endif /* Eba_memset */

#ifndef Eba_need_diy_memset
#define Eba_need_diy_memset 0
#endif

#if Eba_need_diy_memset
void *eba_diy_memset(void *dest, int val, size_t n);
#endif

/**********************************************************************/
/* memcpy */
/**********************************************************************/
/* if you wish, you can define your own memcpy
 * alternatively you may define EBA_DIY_MEMCPY to avoid standard C lib
 * the default is to simply use memcpy from the standard C lib */
#ifndef EBA_DIY_MEMCPY
#define EBA_DIY_MEMCPY 0
#endif

#ifndef Eba_memcpy
#if ((EBA_DIY_MEMCPY) || EBA_SKIP_LIBC)
#define Eba_need_diy_memcpy 1
#define Eba_memcpy eba_diy_memcpy
#else
#include <string.h>
#define Eba_need_diy_memcpy 0
#define Eba_memcpy memcpy
#endif
#endif /* Eba_memcpy */

#ifndef Eba_need_diy_memcpy
#define Eba_need_diy_memcpy 0
#endif

#if Eba_need_diy_memcpy
void *eba_diy_memcpy(void *dest, const void *src, size_t n);
#endif

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
#if Eba_need_shifts
#ifndef Eba_stack_alloc
#if ((Eba_use_libc) && (EBA_NO_ALLOCA || __STDC_NO_VLA__))
#include <stdlib.h>
#define Eba_stack_alloc malloc
#define Eba_stack_alloc_str "malloc"
#define Eba_need_do_stack_free 1
#define Eba_need_no_stack_free 0
#define Eba_stack_free eba_do_stack_free
#else /* ((Eba_use_libc) && (EBA_NO_ALLOCA || __STDC_NO_VLA__)) */
#include <alloca.h>
#define Eba_stack_alloc alloca
#define Eba_stack_alloc_str "alloca"
#define Eba_need_no_stack_free 1
#define Eba_need_do_stack_free 0
#define Eba_stack_free eba_no_stack_free
#endif /* ((Eba_use_libc) && (EBA_NO_ALLOCA || __STDC_NO_VLA__)) */
#endif /* Eba_stack_alloc */
#else /* Eba_need_shifts */
#define Eba_need_no_stack_free 0
#define Eba_need_do_stack_free 0
#endif /* Eba_need_shifts */

/**********************************************************************/
/* constructors */
/**********************************************************************/
#if Eba_need_new
struct eba_s *eba_new(unsigned long num_bits);

#if Eba_need_endian
struct eba_s *eba_new_endian(unsigned long num_bits, enum eba_endian endian);
#endif

void eba_free(struct eba_s *eba);
#endif /* Eba_need_new */

/**********************************************************************/
/* allocation functions */
/**********************************************************************/
#if ((Eba_use_libc) && (Eba_need_new))

/* #define Eba_alloc and Eba_free to use something other than malloc/free */
/* #define Eba_alloc_str for using in error message log lines */

#ifndef Eba_alloc
#include <stdlib.h>
#define Eba_alloc malloc
#define Eba_alloc_str "malloc"
#define Eba_free free
#endif

#ifndef Eba_alloc_str
#define Eba_alloc_str "(custom) allocate"
#endif

#endif /* ((Eba_use_libc) && (Eba_need_new)) */

/**********************************************************************/
/* logging macros and functions */
/**********************************************************************/
/* if no logging macros are defined, we use std C lib
   alternatively define each of:
   Eba_log_error0, Eba_log_error1, Eba_log_error2, Eba_log_error3
*/
#ifndef EBA_SKIP_LOG_ERRORS
#define EBA_SKIP_LOG_ERRORS 0
#endif

#if (EBA_SKIP_LOG_ERRORS || EBA_SKIP_LIBC)
#ifndef Eba_log_nop
#define Eba_log_nop do { /* nothing */ } while (0)
#endif
#ifndef Eba_log_error0
#define Eba_log_error0(format) Eba_log_nop
#define Eba_log_error1(format, arg1) Eba_log_nop
#define Eba_log_error2(format, arg1, arg2) Eba_log_nop
#define Eba_log_error3(format, arg1, arg2, arg3) Eba_log_nop
#endif /* Eba_log_error0 */
#endif /* (EBA_SKIP_LOG_ERRORS || EBA_SKIP_LIBC ) */

#ifndef Eba_log_error0

#define Eba_need_global_log_file 1
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

#else

#define Eba_need_global_log_file 0

#endif /* Eba_log_error0 */

#ifdef __cplusplus
}
#endif

#endif /* EBA_H */
