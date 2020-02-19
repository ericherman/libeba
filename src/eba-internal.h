/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba-internal.h: embedable bit array configuration header */
/* Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#ifndef EBA_INTERNAL_H
#define EBA_INTERNAL_H

#define EBA_NOP do { ((void)0); } while (0)

#ifdef NDEBUG
#define Eba_no_debug 1
#else
#define Eba_no_debug 0
#endif
#define Eba_debug (!(Eba_no_debug))

#ifndef EBA_SKIP_NEW
#define Eba_need_new 1
#else
#define Eba_need_new 0
#endif

#ifndef EBA_SKIP_ENDIAN
#define Eba_need_endian 1
#else
#define Eba_need_endian 0
#endif

#ifndef EBA_SKIP_SHIFTS
#define Eba_need_shifts 1
#else
#define Eba_need_shifts 0
#endif

#ifndef EBA_SKIP_SWAP
#define Eba_need_swap 1
#else
#define Eba_need_swap (Eba_need_shifts)
#endif

#ifndef EBA_SKIP_LIBC
#define Eba_use_libc __STDC_HOSTED__
#else
#define Eba_use_libc 0
#endif

#ifndef Eba_memset
#ifdef EBA_DIY_MEMSET
#define Eba_need_diy_memset 1
#define Eba_memset eba_diy_memset
#endif
#endif

#ifndef Eba_memset
#if Eba_use_libc
#define Eba_need_string_h 1
#define Eba_need_diy_memset 0
#define Eba_memset memset
#else
#define Eba_need_diy_memset 1
#define Eba_memset eba_diy_memset
#endif
#endif

#ifndef Eba_need_diy_memset
#define Eba_need_diy_memset 0
#endif

#ifndef Eba_memcpy
#ifdef EBA_DIY_MEMCPY
#define Eba_need_diy_memcpy 1
#define Eba_memcpy eba_diy_memcpy
#endif
#endif

#ifndef Eba_memcpy
#if Eba_use_libc
#define Eba_need_string_h 1
#define Eba_need_diy_memcpy 0
#define Eba_memcpy memcpy
#else
#define Eba_need_diy_memcpy 1
#define Eba_memcpy eba_diy_memcpy
#endif
#endif

#ifndef Eba_need_diy_memcpy
#define Eba_need_diy_memcpy 0
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
#define Eba_need_stdlib_h 1
#define Eba_stack_alloc(len) Eba_alloc(len)
#define Eba_need_do_stack_free 1
#define Eba_need_no_stack_free 0
#define Eba_stack_free(p) eba_do_stack_free_(p)
#else /* ((Eba_use_libc) && (EBA_NO_ALLOCA || __STDC_NO_VLA__)) */
#define Eba_need_alloca_h 1
#define Eba_stack_alloc(len) alloca(len)
#define Eba_need_no_stack_free 1
#define Eba_need_do_stack_free 0
#define Eba_stack_free(p) eba_no_stack_free_(p)
#endif /* ((Eba_use_libc) && (EBA_NO_ALLOCA || __STDC_NO_VLA__)) */
#endif /* Eba_stack_alloc */
#else /* Eba_need_shifts */
#define Eba_need_no_stack_free 0
#define Eba_need_do_stack_free 0
#endif /* Eba_need_shifts */

/**********************************************************************/
/* allocation functions */
/**********************************************************************/
#if ((Eba_use_libc) && (Eba_need_new))

/* #define Eba_alloc and Eba_free to use something other than malloc/free */

#ifndef Eba_alloc
#define Eba_need_stdlib_h 1
#define Eba_alloc(len) malloc(len)
#define Eba_free(ptr) free(ptr)
#endif

#endif /* ((Eba_use_libc) && (Eba_need_new)) */

#ifndef Eba_need_stdlib_h
#define Eba_need_stdlib_h 0
#endif

#ifndef Eba_need_alloca_h
#define Eba_need_alloca_h 0
#endif

#ifndef Eba_need_string_h
#define Eba_need_string_h 0
#endif

#if Eba_no_debug
#define eba_assert(x) EBA_NOP
#else
#include <assert.h>
#define eba_assert(x) assert(x)
#endif

#if Eba_need_string_h
#include <string.h>
#endif

#if Eba_need_stdlib_h
#include <stdlib.h>
#endif

#if Eba_need_alloca_h
#include <alloca.h>
#endif

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

#endif /* EBA_INTERNAL_H */
