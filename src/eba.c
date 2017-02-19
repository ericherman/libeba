/*
eba.c: embedable bit array - hopefully somewhat suitable for small CPUs
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

#include "eba.h"
#include "eba-log.h"

#ifndef Eba_alloc
#include <stdlib.h>
#define Eba_alloc malloc
#define Eba_free free
#endif

struct eba_s *eba_new(unsigned long num_bits)
{
	struct eba_s *eba;

	eba = Eba_alloc(sizeof(struct eba_s));
	if (!eba) {
		Eba_log_error1("could not allocate %lu bytes?\n",
			       (unsigned long)sizeof(struct eba_s));
		return NULL;
	}

	eba->size = num_bits / 8;
	if ((eba->size * 8) < num_bits) {
		eba->size += 1;
	}

	eba->bits = Eba_alloc(eba->size);
	if (!(eba->bits)) {
		Eba_log_error1("could not allocate %lu bytes?\n",
			       (unsigned long)eba->size);
		Eba_free(eba);
		return NULL;
	}
	return eba;
}

void eba_free(struct eba_s *eba)
{
	if (!eba) {
		return;
	}
	Eba_free(eba->bits);
	Eba_free(eba);
}

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val)
{
	size_t byte;
	unsigned char offset;

	byte = index / 8;	/* compiler should convert to shift */
	offset = index % 8;	/* compiler should convert to bitwise AND */

	if (byte >= eba->size) {
		Eba_log_error3("bit index %lu is position %lu, size is %lu\n",
			       (unsigned long)index, (unsigned long)byte,
			       (unsigned long)eba->size);
		Eba_crash();
	}

	/* This should work, but seems too tricky: */
	/* val = val ? 1 : 0 */ ;
	/* eba->bits[byte] ^= (-val ^ eba->bits[byte]) & (1 << offset); */
	/* instead, be a bit more verbose and trust the optimizer */
	if (val) {
		eba->bits[byte] |= (1 << offset);
	} else {
		eba->bits[byte] &= ~(1 << offset);
	}
}

unsigned char eba_get(struct eba_s *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;

	byte = index / 8;	/* compiler should convert to shift */
	offset = index % 8;	/* compiler should convert to bitwise AND */

	if (byte >= eba->size) {
		Eba_log_error3("bit index %lu is position %lu, size is %lu\n",
			       (unsigned long)index, (unsigned long)byte,
			       (unsigned long)eba->size);
		Eba_crash();
	}

	return (eba->bits[byte] >> offset) & 1;
}
