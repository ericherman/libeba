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

static unsigned char get_byte_and_offset(struct eba_s *eba, unsigned long index,
					 size_t *byte, unsigned char *offset);

#define Get_byte_and_offset(eba, index, byte, offset) \
 if (get_byte_and_offset(eba, index, byte, offset)) { Eba_crash(); }

#define Get_byte_and_offset_uc(eba, index, byte, offset) \
 if (get_byte_and_offset(eba, index, byte, offset)) { Eba_crash_uc(); }

#ifdef EBA_NEED_GLOBAL_LOG_FILE
FILE *eba_global_log_file = NULL;
#endif

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val)
{
	size_t byte;
	unsigned char offset;

	Get_byte_and_offset(eba, index, &byte, &offset);

	/* This should work, but seems too tricky: */
	/* val = val ? 1 : 0 */ ;
	/* eba->bits[byte] ^= (-val ^ eba->bits[byte]) & (1U << offset); */
	/* instead, be a bit more verbose and trust the optimizer */
	if (val) {
		eba->bits[byte] |= (1U << offset);
	} else {
		eba->bits[byte] &= ~(1U << offset);
	}
}

unsigned char eba_get(struct eba_s *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;

	Get_byte_and_offset_uc(eba, index, &byte, &offset);

	return (eba->bits[byte] >> offset) & 1;
}

void eba_toggle(struct eba_s *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;

	Get_byte_and_offset(eba, index, &byte, &offset);

	eba->bits[byte] ^= (1U << offset);
}

#ifndef EBA_SKIP_EBA_NEW
struct eba_s *eba_new(unsigned long num_bits, enum eba_endian endian)
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
	eba->endian = endian;

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
#endif /* EBA_SKIP_EBA_NEW */

static unsigned char get_byte_and_offset(struct eba_s *eba, unsigned long index,
					 size_t *byte, unsigned char *offset)
{

#ifndef EBA_SKIP_STRUCT_NULL_CHECK
	if (!eba) {
		Eba_log_error0("eba struct is NULL\n");
		return 1;
	}
#endif /* EBA_SKIP_STRUCT_NULL_CHECK */

#ifndef EBA_SKIP_STRUCT_BITS_NULL_CHECK
	if (!eba->bits) {
		Eba_log_error0("eba->bits is NULL\n");
		return 1;
	}
#endif /* EBA_SKIP_STRUCT_BITS_NULL_CHECK */

	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / 8;
	*offset = index % 8;

#ifndef EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY
	if ((*byte) >= eba->size) {
		Eba_log_error3("bit index %lu is position %lu, size is %lu\n",
			       (unsigned long)index, (unsigned long)(*byte),
			       (unsigned long)eba->size);
		return 1;
	}
#endif /* EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY */

	if (eba->endian == eba_big_endian) {
		*byte = (eba->size - 1) - (*byte);
	}

	return 0;
}
