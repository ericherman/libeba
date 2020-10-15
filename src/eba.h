/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba.h: embedable bit array - hopefully somewhat suitable for small CPUs */
/* Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#ifndef EBA_H
#define EBA_H 1

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
/**********************************************************************/
/* Avoid using the standard C library? */
/**********************************************************************/
/*
 * When compiling to embedded environments, __STDC_HOSTED__ can be
 * defined to be 1, however due to the increased size of the firmware
 * it may be desirable to _not_ use anything from the libc.
 *
 * If EBA_HOSTED is defined to be 0, then hopefully using EBA
 * will not result in pulling in a bunch of bloat from libc.
 */
/*
 * Function pointers to standard "hosted" LibC functions which may not be
 * available in a freestanding environment. If not hosted, then simple, if
 * not very optimal, versions of these functions will be provided, as they
 * are needed internally and may be useful externally.
 */
/* memset - fill memory with a constant byte */
extern void *(*eba_memset)(void *s, int c, size_t n);

/* memcpy - copy memory area */
extern void *(*eba_memcpy)(void *dest, const void *src, size_t n);

/* alloc, free - allocate and free dynamic memory */
/* for HOSTED, defaults to malloc */
extern void *eba_alloc_context;
extern void *(*eba_alloc)(void *context, size_t size);
extern void (*eba_mfree)(void *context, void *ptr);

extern void (*eba_debug_print_z)(size_t z);
extern void (*eba_debug_print_s)(const char *s);
extern void (*eba_debug_print_eol)(void);
extern void (*eba_debug_die)(void);

/* byte order */
enum eba_endian {
	eba_endian_little = 0,
	eba_big_endian
};

/* embedable bit vector */
struct eba {
	unsigned char *bits;
	size_t size_bytes;
	enum eba_endian endian;
};

/**********************************************************************/
/* essential */
/**********************************************************************/

void eba_set(struct eba *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba *eba, unsigned long index);

/**********************************************************************/
/* constructors */
/**********************************************************************/
struct eba *eba_from_bytes(unsigned char *bytes, size_t len,
			   enum eba_endian endian);

struct eba *eba_new(unsigned long num_bits);

struct eba *eba_new_endian(unsigned long num_bits, enum eba_endian endian);

void eba_free(struct eba *eba);

/**********************************************************************/
/* helper functions */
/**********************************************************************/
char *eba_to_string(struct eba *eba, char *buf, size_t len);

void eba_set_all(struct eba *eba, unsigned char val);

void eba_toggle(struct eba *eba, unsigned long index);

void eba_swap(struct eba *eba, unsigned long index1, unsigned long index2);

int eba_rotate_left(struct eba *eba, unsigned long positions);

int eba_rotate_right(struct eba *eba, unsigned long positions);

int eba_shift_left(struct eba *eba, unsigned long positions);

int eba_shift_right(struct eba *eba, unsigned long positions);

int eba_shift_left_fill(struct eba *eba, unsigned long positions,
			unsigned char fillval);

int eba_shift_right_fill(struct eba *eba, unsigned long positions,
			 unsigned char fillval);

/**********************************************************************/
Eba_end_C_functions
#undef Eba_end_C_functions
#endif /* EBA_H */
