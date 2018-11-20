/*
eba.c: embedable bit array - hopefully somewhat suitable for small CPUs
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

#include "eba.h"

#if ((EBA_SKIP_STRUCT_NULL_CHECK) && (EBA_SKIP_STRUCT_BITS_NULL_CHECK))
#define Is_eba_null(eba) (0)
#else
#define Is_eba_null(eba) is_eba_null(eba)
#define Eba_need_is_eba_null 1
static unsigned char is_eba_null(struct eba_s *eba);
#endif

#if Eba_need_do_stack_free
static void eba_do_stack_free(void *ptr, size_t size);
#endif

#if Eba_need_no_stack_free
static void eba_no_stack_free(void *ptr, size_t size);
#endif

static unsigned char get_byte_and_offset(struct eba_s *eba, unsigned long index,
					 size_t *byte, unsigned char *offset);

#define Get_byte_and_offset(eba, index, byte, offset) \
 if (get_byte_and_offset(eba, index, byte, offset)) { Eba_crash(); }

#define Get_byte_and_offset_uc(eba, index, byte, offset) \
 if (get_byte_and_offset(eba, index, byte, offset)) { Eba_crash_uc(); }

#if Eba_need_global_log_file
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

void eba_set_all(struct eba_s *eba, unsigned char val)
{
	int all_vals;

	if (Is_eba_null(eba)) {
		Eba_crash();
	}

	all_vals = val ? -1 : 0;
	Eba_memset(eba->bits, all_vals, eba->size_bytes);
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

#if Eba_need_shifts

/* since we are allocating on the stack, this can not be a function
   and thus must be a macro such that it exists in the same stack frame */
#define Eba_create_2x_eba_on_stack_inner(eba, tmp, eba_crash_func) do { \
	tmp = (struct eba_s *)Eba_stack_alloc(sizeof(struct eba_s)); \
	if (!tmp) { \
		Eba_log_error2("could not %s %lu bytes?\n",\
			       Eba_stack_alloc_str, \
			       (unsigned long)sizeof(struct eba_s)); \
		eba_crash_func(); \
	} \
	tmp->size_bytes = 0;	/* not really needed, just clarity */ \
	tmp->bits = (unsigned char *)Eba_stack_alloc(2 * eba->size_bytes); \
	if (!tmp->bits) { \
		Eba_log_error2("could not %s %lu bytes?\n", \
			       Eba_stack_alloc_str, \
			       (unsigned long)(2 * eba->size_bytes)); \
		/* we must free before crashing. */ \
		/* Eba_crash may be implemented something like: */ \
		/* { GlobalErr = 1; return; } */ \
		Eba_stack_free(tmp, sizeof(struct eba_s)); \
		eba_crash_func(); \
	} \
	tmp->size_bytes = 2 * eba->size_bytes; \
	/* Eba_memset(tmp->bits, 0, tmp->size_bytes); */ \
	} while (0)

#if Eba_need_endian
#define Eba_create_2x_eba_on_stack(eba, tmp, eba_crash_func) do {\
	Eba_create_2x_eba_on_stack_inner(eba, tmp, eba_crash_func); \
	tmp->endian = eba->endian; \
	} while (0)
#else
#define Eba_create_2x_eba_on_stack(eba, tmp, eba_crash_func) \
	Eba_create_2x_eba_on_stack_inner(eba, tmp, eba_crash_func)
#endif

#define Eba_free_stack_copy(tmp) do { \
	if (tmp) { \
		Eba_stack_free(tmp->bits, tmp->size_bytes); \
		Eba_stack_free(tmp, sizeof(struct eba_s)); \
	} \
	} while (0)

enum eba_shift_fill_val {
	eba_fill_zero = 0,
	eba_fill_one = 1,
	eba_fill_ring
};

static void eba_reverse_bytes_no_null_check(unsigned char *bytes, size_t len)
{
	size_t i, j;
	unsigned char swap;

	for (i = 0, j = len - 1; i < j; ++i, --j) {
		swap = bytes[i];
		bytes[i] = bytes[j];
		bytes[j] = swap;
	}
}

static void eba_inner_shift_right_be(struct eba_s *eba, unsigned long positions,
				     enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba_s *tmp;

	if (positions == 0) {
		return;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			Eba_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	Eba_create_2x_eba_on_stack(eba, tmp, Eba_crash);
	switch (fill) {
	case eba_fill_zero:
		Eba_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		Eba_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		Eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	Eba_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);

	/* Eba_memset(eba->bits, 0, eba->size_bytes); */

	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a = ((unsigned)lo_val) | (((unsigned)hi_val) << 8);
		u16_b = (u16_a >> (positions % 8));
		val = 0xFF & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	Eba_free_stack_copy(tmp);
}

static void eba_inner_shift_right_el(struct eba_s *eba, unsigned long positions,
				     enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba_s *tmp;

	if (Is_eba_null(eba)) {
		Eba_crash();
	}

	if (positions == 0) {
		return;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			Eba_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	Eba_create_2x_eba_on_stack(eba, tmp, Eba_crash);
	switch (fill) {
	case eba_fill_zero:
		Eba_memset(tmp->bits + eba->size_bytes, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		Eba_memset(tmp->bits + eba->size_bytes, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		Eba_memcpy(tmp->bits + eba->size_bytes, eba->bits,
			   eba->size_bytes);
		break;
	};
	Eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
	eba_reverse_bytes_no_null_check(tmp->bits, tmp->size_bytes);

	/* Eba_memset(eba->bits, 0, eba->size_bytes); */

	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a = ((unsigned)lo_val) | (((unsigned)hi_val) << 8);
		u16_b = (u16_a >> (positions % 8));
		val = 0xFF & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	eba_reverse_bytes_no_null_check(eba->bits, eba->size_bytes);

	Eba_free_stack_copy(tmp);
}

static void eba_inner_shift_right(struct eba_s *eba, unsigned long positions,
				  enum eba_shift_fill_val fill)
{
	if (Is_eba_null(eba)) {
		Eba_crash();
	}
#if Eba_need_endian
	if (eba->endian == eba_endian_little) {
		eba_inner_shift_right_el(eba, positions, fill);
	} else {
#endif
		eba_inner_shift_right_be(eba, positions, fill);
#if Eba_need_endian
	}
#endif
}

static void eba_inner_shift_left_be(struct eba_s *eba, unsigned long positions,
				    enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba_s *tmp;

	if (Is_eba_null(eba)) {
		Eba_crash();
	}

	if (positions == 0) {
		return;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			Eba_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	Eba_create_2x_eba_on_stack(eba, tmp, Eba_crash);
	switch (fill) {
	case eba_fill_zero:
		Eba_memset(tmp->bits + eba->size_bytes, 0, tmp->size_bytes);
		break;
	case eba_fill_one:
		Eba_memset(tmp->bits + eba->size_bytes, -1, tmp->size_bytes);
		break;
	case eba_fill_ring:
		Eba_memcpy(tmp->bits + eba->size_bytes, eba->bits,
			   eba->size_bytes);
		break;
	};
	Eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);

	/* Eba_memset(eba->bits, 0, eba->size_bytes); */

	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a = ((unsigned)lo_val) | (((unsigned)hi_val) << 8);
		u16_b = (u16_a << (positions % 8));
		val = ((unsigned char)(u16_b >> 8));
		eba->bits[i] = val;
	}

	Eba_free_stack_copy(tmp);
}

static void eba_inner_shift_left_el(struct eba_s *eba, unsigned long positions,
				    enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba_s *tmp;

	if (Is_eba_null(eba)) {
		Eba_crash();
	}

	if (positions == 0) {
		return;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;

	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			Eba_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	Eba_create_2x_eba_on_stack(eba, tmp, Eba_crash);
	switch (fill) {
	case eba_fill_zero:
		Eba_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		Eba_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		Eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	Eba_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);
	eba_reverse_bytes_no_null_check(tmp->bits, tmp->size_bytes);

	Eba_memset(eba->bits, 0, eba->size_bytes);

	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a = ((unsigned)lo_val) | (((unsigned)hi_val) << 8);
		u16_b = (u16_a << (positions % 8));
		val = ((unsigned char)(u16_b >> 8));
		eba->bits[i] = val;
	}

	eba_reverse_bytes_no_null_check(eba->bits, eba->size_bytes);

	Eba_free_stack_copy(tmp);
}

static void eba_inner_shift_left(struct eba_s *eba, unsigned long positions,
				 enum eba_shift_fill_val fill)
{
	if (Is_eba_null(eba)) {
		Eba_crash();
	}
#if Eba_need_endian
	if (eba->endian == eba_endian_little) {
		eba_inner_shift_left_el(eba, positions, fill);
	} else {
#endif
		eba_inner_shift_left_be(eba, positions, fill);
#if Eba_need_endian
	}
#endif
}

void eba_ring_shift_right(struct eba_s *eba, unsigned long positions)
{
	eba_inner_shift_right(eba, positions, eba_fill_ring);
}

void eba_ring_shift_left(struct eba_s *eba, unsigned long positions)
{
	eba_inner_shift_left(eba, positions, eba_fill_ring);
}

void eba_shift_left(struct eba_s *eba, unsigned long positions)
{
	eba_inner_shift_left(eba, positions, eba_fill_zero);
}

void eba_shift_left_fill(struct eba_s *eba, unsigned long positions,
			 unsigned char fillval)
{
	eba_inner_shift_left(eba, positions,
			     fillval ? eba_fill_one : eba_fill_zero);
}

void eba_shift_right(struct eba_s *eba, unsigned long positions)
{
	eba_inner_shift_right(eba, positions, eba_fill_zero);
}

void eba_shift_right_fill(struct eba_s *eba, unsigned long positions,
			  unsigned char fillval)
{
	eba_inner_shift_right(eba, positions,
			      fillval ? eba_fill_one : eba_fill_zero);
}

#endif /* EBA_SKIP_SHIFTS */

#if Eba_need_new

#if Eba_need_endian
struct eba_s *eba_new_endian(unsigned long num_bits, enum eba_endian endian)
#else
struct eba_s *eba_new(unsigned long num_bits)
#endif
{
	struct eba_s *eba;

	eba = (struct eba_s *)Eba_alloc(sizeof(struct eba_s));
	if (!eba) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_alloc_str,
			       (unsigned long)sizeof(struct eba_s));
		return NULL;
	}

	eba->size_bytes = num_bits / EBA_CHAR_BIT;
	if ((eba->size_bytes * EBA_CHAR_BIT) < num_bits) {
		eba->size_bytes += 1;
	}
#if Eba_need_endian
	eba->endian = endian;
#endif

	eba->bits = (unsigned char *)Eba_alloc(eba->size_bytes);
	if (!(eba->bits)) {
		Eba_log_error2("could not %s %lu bytes?\n", Eba_alloc_str,
			       (unsigned long)eba->size_bytes);
		Eba_free(eba);
		return NULL;
	}

	return eba;
}

#if Eba_need_endian
struct eba_s *eba_new(unsigned long num_bits)
{
	return eba_new_endian(num_bits, eba_big_endian);
}
#endif

void eba_free(struct eba_s *eba)
{
	if (!eba) {
		return;
	}
	if (eba->bits) {
		Eba_free(eba->bits);
	} else {
		Eba_log_error0("eba->bits is NULL\n");
	}
	Eba_free(eba);
}
#endif /* Eba_need_new */

#if Eba_need_is_eba_null
static unsigned char is_eba_null(struct eba_s *eba)
{
#if Eba_need_struct_null_check
	if (!eba) {
		Eba_log_error0("eba struct is NULL\n");
		return 1;
	}
#endif /* Eba_need_struct_null_check */

#if Eba_need_struct_bits_null_check
	if (!eba->bits) {
		Eba_log_error0("eba->bits is NULL\n");
		return 1;
	}
#endif /* Eba_need_struct_bits_null_check */

	return 0;
}
#endif /* Eba_need_is_eba_null */

static unsigned char get_byte_and_offset(struct eba_s *eba, unsigned long index,
					 size_t *byte, unsigned char *offset)
{
	if (Is_eba_null(eba)) {
		return 1;
	}

	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / EBA_CHAR_BIT;
	*offset = index % EBA_CHAR_BIT;

#if Eba_need_array_index_overrun_safety
	if ((*byte) >= eba->size_bytes) {
		Eba_log_error3("bit index %lu is position %lu, size is %lu\n",
			       (unsigned long)index, (unsigned long)(*byte),
			       (unsigned long)eba->size_bytes);
		return 1;
	}
#endif /* Eba_need_array_index_overrun_safety */

#if Eba_need_endian
	if (eba->endian == eba_big_endian) {
#endif
		*byte = (eba->size_bytes - 1) - (*byte);
#if Eba_need_endian
	}
#endif

	return 0;
}

#if Eba_need_do_stack_free
static void eba_do_stack_free(void *ptr, size_t size)
{
#if (!(NDEBUG))
	if (size == 0) {
		Eba_log_error2("size is 0? (%p, %lu)\n", ptr,
			       (unsigned long)size);
	}
#endif /* NDEBUG */
	free(ptr);
}
#endif

#if Eba_need_no_stack_free
static void eba_no_stack_free(void *ptr, size_t size)
{
#ifndef NDEBUG
	if (size == 0) {
		Eba_log_error2("size is 0? (%p, %lu)\n", ptr,
			       (unsigned long)size);
	}
#endif /* NDEBUG */
}
#endif

#if Eba_need_diy_memcpy
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

#if Eba_need_diy_memset
void *eba_diy_memset(void *dest, int val, size_t n)
{
	unsigned char *d;
	unsigned char v;

	if (dest) {
		d = (unsigned char *)dest;
		v = (unsigned char)val;
		while (n--) {
			d[n] = v;
		}
	}
	return dest;
}
#endif

char *eba_to_string(struct eba_s *eba, char *buf, size_t len)
{
	size_t i, pos, size_bits, done;

	if (!buf) {
		return NULL;
	}
	if (len < 2) {
		return NULL;
	}
	buf[0] = '\0';

	if (Is_eba_null(eba)) {
		return buf;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	pos = 0;
	done = 0;

#if Eba_need_endian
	if (eba->endian == eba_endian_little) {
		for (i = 0; pos < (len - 1) && i < size_bits; ++i) {
			++done;
			buf[pos++] = eba_get(eba, i) ? '1' : '0';
			if ((done % 8 == 0) && (i < (size_bits - 1))
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	} else {
#endif
		for (i = size_bits; pos < (len - 1) && i; --i) {
			++done;
			buf[pos++] = eba_get(eba, (i - 1)) ? '1' : '0';
			if ((done % 8 == 0) && (i - 1) && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
#if Eba_need_endian
	}
#endif
	if ((pos < len) && (done == size_bits)) {
		buf[pos] = '\0';
		return buf;
	}

	if (pos < (len - 1)) {
		buf[pos++] = '!';
		buf[pos] = '\0';
		return buf;
	}

	buf[len - 2] = '!';
	buf[len - 1] = '\0';
	return buf;
}
