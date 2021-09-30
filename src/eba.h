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
/* Basic bit extraction or insertion into a byte */
unsigned char eba_get_byte_bit(unsigned char byte, unsigned i);
unsigned char eba_set_byte_bit(unsigned char byte, unsigned i, unsigned val);

/* forward declaration of main eba struct */
struct eba;

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

void eba_rotate_left(struct eba *eba, unsigned long positions);

void eba_rotate_right(struct eba *eba, unsigned long positions);

void eba_shift_left(struct eba *eba, unsigned long positions);

void eba_shift_right(struct eba *eba, unsigned long positions);

void eba_shift_left_fill(struct eba *eba, unsigned long positions,
			 unsigned char fillval);

void eba_shift_right_fill(struct eba *eba, unsigned long positions,
			  unsigned char fillval);

/**********************************************************************/
Eba_end_C_functions
#undef Eba_end_C_functions
#endif /* EBA_H */
