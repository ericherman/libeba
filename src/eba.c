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

static unsigned char is_eba_null(struct eba_s *eba);

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

void eba_swap(struct eba_s *eba, unsigned long index1, unsigned long index2)
{
	unsigned char tmp;

	tmp = eba_get(eba, index1);
	eba_set(eba, index1, eba_get(eba, index2));
	eba_set(eba, index2, tmp);
}

void eba_ring_shift_right(struct eba_s *eba, unsigned long positions)
{
	unsigned long i, j, size_bits;
	unsigned char val;
	struct eba_s *tmp;

	if (is_eba_null(eba)) {
		Eba_crash();
	}

	size_bits = eba->size_bytes * 8;

	if (positions >= size_bits) {
		positions = positions % size_bits;
	}

	if (positions == 0) {
		return;
	}

	tmp = (struct eba_s *)Eba_stack_alloc(sizeof(struct eba_s));
	if (!tmp) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_stack_alloc_str,
			       sizeof(struct eba_s));
		Eba_crash();
	}
	tmp->endian = eba->endian;
	tmp->size_bytes = 0;	/* not really needed, just clarity */
	tmp->bits = (unsigned char *)Eba_stack_alloc(eba->size_bytes);
	if (!tmp->bits) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_stack_alloc_str,
			       (unsigned long)eba->size_bytes);
		/* we must free before crashing. */
		/* Eba_crash may be implemented something like: */
		/* { GlobalErr = 1; return; } */
		Eba_stack_free(tmp, sizeof(struct eba_s));
		Eba_crash();
	}
	tmp->size_bytes = eba->size_bytes;
	Eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);

	for (i = 0; i < size_bits; ++i) {
		j = i + positions;
		if (j >= size_bits) {
			j -= size_bits;
		}
		val = eba_get(tmp, j);
		eba_set(eba, i, val);
	}
	Eba_stack_free(tmp->bits, tmp->size_bytes);
	Eba_stack_free(tmp, sizeof(struct eba_s));
}

void eba_ring_shift_left(struct eba_s *eba, unsigned long positions)
{
	unsigned long size_bits;

	if (is_eba_null(eba)) {
		Eba_crash();
	}

	size_bits = eba->size_bytes * 8;

	if (positions >= size_bits) {
		positions = positions % size_bits;
	}
	eba_ring_shift_right(eba, size_bits - positions);
}

#ifndef EBA_SKIP_EBA_NEW
struct eba_s *eba_new(unsigned long num_bits, enum eba_endian endian)
{
	struct eba_s *eba;

	eba = Eba_alloc(sizeof(struct eba_s));
	if (!eba) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_alloc_str,
			       (unsigned long)sizeof(struct eba_s));
		return NULL;
	}

	eba->size_bytes = num_bits / 8;
	if ((eba->size_bytes * 8) < num_bits) {
		eba->size_bytes += 1;
	}
	eba->endian = endian;

	eba->bits = Eba_alloc(eba->size_bytes);
	if (!(eba->bits)) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_alloc_str,
			       (unsigned long)eba->size_bytes);
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

static unsigned char is_eba_null(struct eba_s *eba)
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

	return 0;
}

static unsigned char get_byte_and_offset(struct eba_s *eba, unsigned long index,
					 size_t *byte, unsigned char *offset)
{
	if (is_eba_null(eba)) {
		return 1;
	}

	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / 8;
	*offset = index % 8;

#ifndef EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY
	if ((*byte) >= eba->size_bytes) {
		Eba_log_error3("bit index %lu is position %lu, size is %lu\n",
			       (unsigned long)index, (unsigned long)(*byte),
			       (unsigned long)eba->size_bytes);
		return 1;
	}
#endif /* EBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY */

	if (eba->endian == eba_big_endian) {
		*byte = (eba->size_bytes - 1) - (*byte);
	}

	return 0;
}

#ifdef EBA_NEED_DO_STACK_FREE
void eba_do_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		Eba_log_error2("size is 0? (%p, %lu)\n", ptr,
			       (unsigned long)size);
	}
	free(ptr);
}
#endif

#ifdef EBA_NEED_NO_STACK_FREE
void eba_no_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		Eba_log_error2("size is 0? (%p, %lu)\n", ptr,
			       (unsigned long)size);
	}
}
#endif

#ifdef EBA_NEED_DIY_MEMCPY
void *eba_diy_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d;
	const unsigned char *s;

	if (dest) {

		d = (unsigned char *)dest;
		s = (const unsigned char *)src;
		while (n--) {
			d[n] = s[n];
		}
	}
	return dest;
}
#endif
