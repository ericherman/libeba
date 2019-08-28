/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba.c: embedable bit array - hopefully somewhat suitable for small CPUs */
/* Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba.h"
#include "eba-internal.h"

static int eba_is_endian_little_(struct eba_s *eba);

#if Eba_need_do_stack_free
static void eba_do_stack_free_(void *ptr);
#endif

#if Eba_need_no_stack_free
#if (Eba_debug)
static void eba_no_stack_free_(void *ptr);
#else
#define eba_no_stack_free_(ptr) EBA_NOP
#endif /* (Eba_debug) */
#endif

#if (Eba_debug)
static void eba_assert_not_null_(struct eba_s *eba)
{
	eba_assert(eba);
	eba_assert(eba->bits);
	eba_assert(eba->size_bytes);
}
#else
#define eba_assert_not_null_(eba) EBA_NOP
#endif

static void eba_get_byte_and_offset_(struct eba_s *eba, unsigned long index,
				     size_t *byte, unsigned char *offset);

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val)
{
	size_t byte;
	unsigned char offset;

	eba_assert_not_null_(eba);

	eba_get_byte_and_offset_(eba, index, &byte, &offset);

	/* This should work, but seems too tricky: */
	/* val = val ? 1 : 0; */
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

	eba_assert_not_null_(eba);

	eba_get_byte_and_offset_(eba, index, &byte, &offset);

	return (eba->bits[byte] >> offset) & 0x01;
}

#ifndef EBA_SKIP_SET_ALL
void eba_set_all(struct eba_s *eba, unsigned char val)
{
	int all_vals;

	eba_assert_not_null_(eba);

	all_vals = val ? -1 : 0;
	Eba_memset(eba->bits, all_vals, eba->size_bytes);
}
#endif /* EBA_SKIP_SET_ALL */

#if (!(EBA_SKIP_TOGGLE))
void eba_toggle(struct eba_s *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;

	eba_assert_not_null_(eba);

	eba_get_byte_and_offset_(eba, index, &byte, &offset);

	eba->bits[byte] ^= (1U << offset);
}
#endif /* (!(EBA_SKIP_TOGGLE)) */

#if Eba_need_swap
void eba_swap(struct eba_s *eba, unsigned long index1, unsigned long index2)
{
	unsigned char tmp1, tmp2;

	tmp1 = eba_get(eba, index1);
	tmp2 = eba_get(eba, index2);

	eba_set(eba, index1, tmp2);
	eba_set(eba, index2, tmp1);
}
#endif /* Eba_need_swap */

#if Eba_need_shifts

/* since we are allocating on the stack, this can not be a function
   and thus must be a macro such that it exists in the same stack frame */
#define Eba_create_2x_eba_on_stack(eba, tmp) do { \
	tmp = (struct eba_s *)Eba_stack_alloc(sizeof(struct eba_s)); \
	eba_assert(tmp); \
	tmp->size_bytes = 0;	/* not really needed, just clarity */ \
	tmp->bits = (unsigned char *)Eba_stack_alloc(2 * eba->size_bytes); \
	eba_assert(tmp->bits); \
	tmp->size_bytes = 2 * eba->size_bytes; \
	tmp->endian = eba->endian; \
	/* Eba_memset(tmp->bits, 0, tmp->size_bytes); */ \
	} while (0)

#define Eba_free_stack_copy(tmp) do { \
	if (tmp) { \
		Eba_stack_free(tmp->bits); \
		Eba_stack_free(tmp); \
	} \
	} while (0)

enum eba_shift_fill_val {
	eba_fill_zero = 0,
	eba_fill_one = 1,
	eba_fill_ring
};

static void eba_reverse_bytes_(unsigned char *bytes, size_t len)
{
	size_t i, j;
	unsigned char swap;

	eba_assert(bytes);
	eba_assert(len);

	for (i = 0, j = len - 1; i < j; ++i, --j) {
		swap = bytes[i];
		bytes[i] = bytes[j];
		bytes[j] = swap;
	}
}

static void eba_shift_right_be_(struct eba_s *eba, unsigned long positions,
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

	Eba_create_2x_eba_on_stack(eba, tmp);
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

static void eba_shift_right_el_(struct eba_s *eba, unsigned long positions,
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

	Eba_create_2x_eba_on_stack(eba, tmp);
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
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);

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

	eba_reverse_bytes_(eba->bits, eba->size_bytes);

	Eba_free_stack_copy(tmp);
}

static void eba_shift_right_(struct eba_s *eba, unsigned long positions,
			     enum eba_shift_fill_val fill)
{
	if (eba_is_endian_little_(eba)) {
		eba_shift_right_el_(eba, positions, fill);
	} else {
		eba_shift_right_be_(eba, positions, fill);
	}
}

static void eba_shift_left_be_(struct eba_s *eba, unsigned long positions,
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

	Eba_create_2x_eba_on_stack(eba, tmp);
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

static void eba_shift_left_el_(struct eba_s *eba, unsigned long positions,
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

	Eba_create_2x_eba_on_stack(eba, tmp);
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
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);

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

	eba_reverse_bytes_(eba->bits, eba->size_bytes);

	Eba_free_stack_copy(tmp);
}

static void eba_shift_left_(struct eba_s *eba, unsigned long positions,
			    enum eba_shift_fill_val fill)
{
	if (eba_is_endian_little_(eba)) {
		eba_shift_left_el_(eba, positions, fill);
	} else {
		eba_shift_left_be_(eba, positions, fill);
	}
}

void eba_ring_shift_right(struct eba_s *eba, unsigned long positions)
{
	eba_shift_right_(eba, positions, eba_fill_ring);
}

void eba_ring_shift_left(struct eba_s *eba, unsigned long positions)
{
	eba_shift_left_(eba, positions, eba_fill_ring);
}

void eba_shift_left(struct eba_s *eba, unsigned long positions)
{
	eba_shift_left_(eba, positions, eba_fill_zero);
}

void eba_shift_left_fill(struct eba_s *eba, unsigned long positions,
			 unsigned char fillval)
{
	eba_shift_left_(eba, positions, fillval ? eba_fill_one : eba_fill_zero);
}

void eba_shift_right(struct eba_s *eba, unsigned long positions)
{
	eba_shift_right_(eba, positions, eba_fill_zero);
}

void eba_shift_right_fill(struct eba_s *eba, unsigned long positions,
			  unsigned char fillval)
{
	eba_shift_right_(eba, positions,
			 fillval ? eba_fill_one : eba_fill_zero);
}

#endif /* Eba_need_shifts */

#if Eba_need_new

struct eba_s *eba_new_endian(unsigned long num_bits, enum eba_endian endian)
{
	struct eba_s *eba;

	if ((!Eba_need_endian)) {
		eba_assert(endian == eba_big_endian);
	}

	eba = (struct eba_s *)Eba_alloc(sizeof(struct eba_s));
	if (!eba) {
		return NULL;
	}

	eba->size_bytes = num_bits / EBA_CHAR_BIT;
	if ((eba->size_bytes * EBA_CHAR_BIT) < num_bits) {
		eba->size_bytes += 1;
	}
	eba->endian = endian;

	eba->bits = (unsigned char *)Eba_alloc(eba->size_bytes);
	if (!(eba->bits)) {
		Eba_free(eba);
		return NULL;
	}

	return eba;
}

struct eba_s *eba_new(unsigned long num_bits)
{
	return eba_new_endian(num_bits, eba_big_endian);
}

void eba_free(struct eba_s *eba)
{
	if (!eba) {
		return;
	}
	if (eba->bits) {
		Eba_free(eba->bits);
	}
	Eba_free(eba);
}
#endif /* Eba_need_new */

static int eba_is_endian_little_(struct eba_s *eba)
{
	return (Eba_need_endian && eba->endian == eba_endian_little) ? 1 : 0;
}

static void eba_get_byte_and_offset_(struct eba_s *eba, unsigned long index,
				     size_t *byte, unsigned char *offset)
{
	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / EBA_CHAR_BIT;
	*offset = index % EBA_CHAR_BIT;

	eba_assert((*byte) < eba->size_bytes);

	if (!eba_is_endian_little_(eba)) {
		*byte = (eba->size_bytes - 1) - (*byte);
	}
}

#if Eba_need_do_stack_free
static void eba_do_stack_free_(void *ptr)
{
	eba_assert(ptr);
	free(ptr);
}
#endif

#if ((Eba_need_no_stack_free) && (Eba_debug))
static void eba_no_stack_free_(void *ptr)
{
	eba_assert(ptr);
}
#endif

#if Eba_need_diy_memcpy
void *eba_diy_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d;
	const unsigned char *s;

	eba_assert(dest);

	if (!n) {
		return dest;
	}

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;

	while (n--) {
		d[n] = s[n];
	}

	return dest;
}
#endif

#if Eba_need_diy_memset
void *eba_diy_memset(void *dest, int val, size_t n)
{
	unsigned char *d;
	unsigned char v;

	eba_assert(dest);

	if (!n) {
		return dest;
	}

	d = (unsigned char *)dest;
	v = (unsigned char)val;
	while (n--) {
		d[n] = v;
	}

	return dest;
}
#endif

#ifndef EBA_SKIP_TO_STRING
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

	if (!eba || !eba->bits) {
		return buf;
	}

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	pos = 0;
	done = 0;

	if (eba_is_endian_little_(eba)) {
		for (i = 0; pos < (len - 1) && i < size_bits; ++i) {
			++done;
			buf[pos++] = eba_get(eba, i) ? '1' : '0';
			if ((done % 8 == 0) && (i < (size_bits - 1))
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	} else {
		for (i = size_bits; pos < (len - 1) && i; --i) {
			++done;
			buf[pos++] = eba_get(eba, (i - 1)) ? '1' : '0';
			if ((done % 8 == 0) && (i - 1) && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	}

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
#endif /* EBA_SKIP_TO_STRING */
