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

/* prior to including eba.h, you may wish to #define some things */

/**********************************************************************/
/* Avoid using the standard C library? */
/**********************************************************************/
/*
 * When compiling to embedded environments, __STDC_HOSTED__ can be
 * defined to be 1, however due to the increased size of the firmware
 * it may be desirable to _not_ use anything from the libc.
 *
 * If EBA_SKIP_LIBC is defined, then hopefully using EBA
 * will not result in pulling in a bunch of bloat from libc.
 */

/**********************************************************************/
/* memset */
/**********************************************************************/
/* if you wish, you can define your own memset:
 *	#define memset(target, val, len) custom_memset(target, val, len)
 *
 * alternatively you may define EBA_DIY_MEMSET to avoid standard C lib
 * the default is to simply use memset from the standard C lib */

/**********************************************************************/
/* memcpy */
/**********************************************************************/
/* if you wish, you can define your own memcpy:
 *	#define memcpy(dest, src, len) custom_memcpy(dest, src, len)
 *
 * alternatively you may define EBA_DIY_MEMCPY to avoid standard C lib
 * the default is to simply use memcpy from the standard C lib */

/**********************************************************************/
/* allocation functions */
/**********************************************************************/
/* #define Eba_alloc(size) and Eba_free(ptr) */

/**********************************************************************/
/* internal stack allocation functions */
/**********************************************************************/
/* if you wish to avoid stack allocation, define EBA_NO_ALLOCA */

/**********************************************************************/

#ifdef __cplusplus
#define Eba_begin_C_functions extern "C" {
#define Eba_end_C_functions }
#else
#define Eba_begin_C_functions
#define Eba_end_C_functions
#endif

/**********************************************************************/
Eba_begin_C_functions
#undef Eba_begin_C_functions
/**********************************************************************/
#include <stddef.h>		/* size_t */
/* byte order */
    enum eba_endian {
	eba_endian_little = 0,
	eba_big_endian
};

/* embedable bit vector */
struct eba_s {
	unsigned char *bits;
	size_t size_bytes;
	enum eba_endian endian;
};

/**********************************************************************/
/* essential */
/**********************************************************************/

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba_s *eba, unsigned long index);

/**********************************************************************/
/* constructors */
/**********************************************************************/
#ifndef EBA_SKIP_NEW
struct eba_s *eba_new(unsigned long num_bits);

struct eba_s *eba_new_endian(unsigned long num_bits, enum eba_endian endian);

void eba_free(struct eba_s *eba);
#endif /* EBA_SKIP_NEW */

#ifndef EBA_SKIP_TO_STRING
char *eba_to_string(struct eba_s *eba, char *buf, size_t len);
#endif

#ifndef EBA_SKIP_SET_ALL
void eba_set_all(struct eba_s *eba, unsigned char val);
#endif

#ifndef EBA_SKIP_TOGGLE
void eba_toggle(struct eba_s *eba, unsigned long index);
#endif

#ifndef EBA_SKIP_SWAP
void eba_swap(struct eba_s *eba, unsigned long index1, unsigned long index2);
#endif

#ifndef EBA_SKIP_SHIFTS
void eba_ring_shift_left(struct eba_s *eba, unsigned long positions);

void eba_ring_shift_right(struct eba_s *eba, unsigned long positions);

void eba_shift_left(struct eba_s *eba, unsigned long positions);

void eba_shift_right(struct eba_s *eba, unsigned long positions);

void eba_shift_left_fill(struct eba_s *eba, unsigned long positions,
			 unsigned char fillval);

void eba_shift_right_fill(struct eba_s *eba, unsigned long positions,
			  unsigned char fillval);
#endif /* EBA_SKIP_SHIFTS */

#ifdef EBA_DIY_MEMSET
void *eba_diy_memset(void *dest, int val, size_t n);
#endif

#ifdef EBA_DIY_MEMCPY
void *eba_diy_memcpy(void *dest, const void *src, size_t n);
#endif

/**********************************************************************/
Eba_end_C_functions
#undef Eba_end_C_functions
#endif /* EBA_H */
