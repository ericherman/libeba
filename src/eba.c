/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba.c: embedable bit array - hopefully somewhat suitable for small CPUs */
/* Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba.h"
#include "eembed.h"

#include <limits.h>

#ifndef EBA_DEBUG
#ifdef NDEBUG
#define EBA_DEBUG 0
#elif EEMBED_HOSTED
#define EBA_DEBUG 1
#elif FAUX_FREESTANDING
#define EBA_DEBUG 1
#else
#define EBA_DEBUG 0
#endif
#endif

#ifndef EBA_SKIP_NEW
#define EBA_SKIP_NEW 0
#endif

#ifndef EBA_SKIP_SHIFTS
#define EBA_SKIP_SHIFTS 0
#endif

#ifndef EBA_SKIP_SWAP
#define EBA_SKIP_SWAP 0
#endif

#ifndef EBA_SKIP_TO_STRING
#define EBA_SKIP_TO_STRING 0
#endif

#if (EBA_DEBUG)
static void eba_assert_not_null_(struct eba *eba)
{
	eembed_assert(eba);
	eembed_assert(eba->bits);
	eembed_assert(eba->size_bytes);
}
#else
#define eba_assert_not_null_(eba) EEMBED_NOP()
#endif

static void eba_get_byte_and_offset_(struct eba *eba, unsigned long index,
				     size_t *byte, unsigned char *offset);

void eba_set(struct eba *eba, unsigned long index, unsigned char val)
{
	size_t byte = 0;
	unsigned char offset = 0;

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

unsigned char eba_get(struct eba *eba, unsigned long index)
{
	size_t byte = 0;
	unsigned char offset = 0;

	eba_assert_not_null_(eba);

	eba_get_byte_and_offset_(eba, index, &byte, &offset);
	return (eba->bits[byte] >> offset) & 0x01;
}

#if (!(EBA_SKIP_SET_ALL))
void eba_set_all(struct eba *eba, unsigned char val)
{
	int all_vals = val ? -1 : 0;

	eba_assert_not_null_(eba);

	eembed_memset(eba->bits, all_vals, eba->size_bytes);
}
#endif /* (!(EBA_SKIP_SET_ALL)) */

#if (!(EBA_SKIP_TOGGLE))
void eba_toggle(struct eba *eba, unsigned long index)
{
	size_t byte = 0;
	unsigned char offset = 0;

	eba_assert_not_null_(eba);

	eba_get_byte_and_offset_(eba, index, &byte, &offset);
	eba->bits[byte] ^= (1U << offset);
}
#endif /* (!(EBA_SKIP_TOGGLE)) */

#if (!(EBA_SKIP_SWAP))
void eba_swap(struct eba *eba, unsigned long index1, unsigned long index2)
{
	unsigned char tmp1 = 0;
	unsigned char tmp2 = 0;

	tmp1 = eba_get(eba, index1);
	tmp2 = eba_get(eba, index2);
	eba_set(eba, index1, tmp2);
	eba_set(eba, index2, tmp1);
}
#endif /* (!(EBA_SKIP_SWAP)) */

struct eba *eba_from_bytes(unsigned char *bytes, size_t len,
			   enum eba_endian endian)
{
	struct eba *eba = NULL;
	unsigned char *space = NULL;
	size_t space_len = 0;

	if (!bytes) {
		return NULL;
	}

	if (len < sizeof(struct eba)) {
		return NULL;
	}

	eba = (struct eba *)bytes;
	space = bytes + (sizeof(struct eba));
	space_len = len - (sizeof(struct eba));

	eba->endian = endian;
	eba->size_bytes = space_len;
	eba->bits = space;

	return eba;
}

#if (!(EBA_SKIP_SHIFTS))

enum eba_shift_fill_val {
	eba_fill_zero = 0,
	eba_fill_one = 1,
	eba_fill_ring
};

#ifndef Eba_stack_buf_size
#define Eba_stack_buf_size ((sizeof(void *)) * (sizeof(void *)))
#endif

static size_t eba_min_(size_t a, size_t b)
{
	return a > b ? b : a;
}

static void eba_shift_right_be_(struct eba *eba, unsigned long positions,
				enum eba_shift_fill_val fill)
{
	unsigned char *buf = NULL;
	size_t i = 0;
	size_t buf_size = 0;
	size_t shift_bytes = 0;
	size_t shift_bits = 0;
	size_t size_bits = 0;
	unsigned char lowest = 0;
	size_t tmp_size = Eba_stack_buf_size;
	unsigned char tmp[Eba_stack_buf_size];

	eembed_memset(tmp, (fill == eba_fill_one ? 0xFF : 0x00), tmp_size);

	eba_assert_not_null_(eba);
	eembed_assert(CHAR_BIT == 8);
	eembed_assert(eba->size_bytes);
	eembed_assert(eba->size_bytes < (ULONG_MAX * 8));

	size_bits = eba->size_bytes * CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			int all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	if (positions == 0) {
		return;
	}

	buf = eba->bits;
	buf_size = eba->size_bytes;

	shift_bytes = positions / 8;
	while (shift_bytes) {
		size_t mv_bytes = eba_min_(shift_bytes, tmp_size);

		if (fill == eba_fill_ring) {
			unsigned char *orig = buf + (buf_size - mv_bytes);
			eembed_memcpy(tmp, orig, mv_bytes);
		};

		eembed_memmove(buf + mv_bytes, buf, buf_size - mv_bytes);
		eembed_memcpy(buf, tmp, mv_bytes);

		shift_bytes -= mv_bytes;
	}

	shift_bits = positions % CHAR_BIT;
	if (shift_bits == 0) {
		return;
	}

	switch (fill) {
	case eba_fill_zero:
		lowest = 0x00;
		break;
	case eba_fill_one:
		lowest = 0xFF;
		break;
	case eba_fill_ring:
		lowest = buf[buf_size - 1];
		break;
	};
	for (i = 0; i < buf_size; ++i) {
		size_t byte_pos = buf_size - (1 + i);
		unsigned char lo_val = buf[byte_pos];
		unsigned char hi_val = byte_pos ? buf[byte_pos - 1] : lowest;
		unsigned u16_a = (((unsigned)hi_val) << 8) | ((unsigned)lo_val);
		unsigned u16_b = (u16_a >> shift_bits);
		unsigned char val = 0xFF & ((unsigned char)u16_b);
		buf[byte_pos] = val;
	}
}

static void eba_shift_right_el_(struct eba *eba, unsigned long positions,
				enum eba_shift_fill_val fill)
{
	unsigned long i = 0;
	size_t shift_bytes = 0;
	unsigned char lowest = 0x00;
	size_t size_bits = 0;
	size_t shift_bits = 0;
	unsigned char *buf = NULL;
	size_t buf_size = 0;
	size_t tmp_size = Eba_stack_buf_size;
	unsigned char tmp[Eba_stack_buf_size];

	eembed_memset(tmp, (fill == eba_fill_one ? 0xFF : 0x00), tmp_size);

	buf = eba->bits;
	buf_size = eba->size_bytes;
	size_bits = eba->size_bytes * CHAR_BIT;

	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			int all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	if (positions == 0) {
		return;
	}

	shift_bytes = positions / CHAR_BIT;
	while (shift_bytes) {
		size_t mv_bytes = eba_min_(shift_bytes, tmp_size);
		if (fill == eba_fill_ring) {
			eembed_memcpy(tmp, buf, mv_bytes);
		}
		eembed_memmove(buf, buf + mv_bytes, buf_size - mv_bytes);
		eembed_memcpy(buf + buf_size - mv_bytes, tmp, mv_bytes);

		shift_bytes -= mv_bytes;
	}

	shift_bits = positions % CHAR_BIT;
	if (shift_bits == 0) {
		return;
	}

	switch (fill) {
	case eba_fill_zero:
		lowest = 0x00;
		break;
	case eba_fill_one:
		lowest = 0xFF;
		break;
	case eba_fill_ring:
		lowest = buf[0];
		break;
	}
	for (i = 0; i < buf_size; ++i) {
		unsigned char hi_val = i < (buf_size - 1) ? buf[i + 1] : lowest;
		unsigned char lo_val = buf[i];
		unsigned u16_a = (((unsigned)hi_val) << 8) | ((unsigned)lo_val);
		unsigned u16_b = (u16_a >> shift_bits);
		unsigned char val = 0xFF & ((unsigned char)u16_b);
		buf[i] = val;
	}
	return;
}

static void eba_shift_right_(struct eba *eba, unsigned long positions,
			     enum eba_shift_fill_val fill)
{
	if (eba->endian == eba_big_endian) {
		eba_shift_right_be_(eba, positions, fill);
	} else {
		eba_shift_right_el_(eba, positions, fill);
	}
}

static void eba_shift_left_be_(struct eba *eba, unsigned long positions,
			       enum eba_shift_fill_val fill)
{
	unsigned long i = 0;
	size_t shift_bytes = 0;
	unsigned char highest = 0x00;
	size_t shift_bits = 0;
	size_t size_bits = 0;
	unsigned char *buf = NULL;
	size_t buf_size = 0;
	size_t tmp_size = Eba_stack_buf_size;
	unsigned char tmp[Eba_stack_buf_size];

	eembed_memset(tmp, (fill == eba_fill_one ? 0xFF : 0x00), tmp_size);

	size_bits = eba->size_bytes * CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			int all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	if (positions == 0) {
		return;
	}

	buf = eba->bits;
	buf_size = eba->size_bytes;

	shift_bytes = positions / CHAR_BIT;
	while (shift_bytes) {
		size_t mv_bytes = eba_min_(shift_bytes, tmp_size);
		if (fill == eba_fill_ring) {
			eembed_memcpy(tmp, buf, mv_bytes);
		}
		eembed_memmove(buf, buf + mv_bytes, buf_size - mv_bytes);
		eembed_memcpy(buf + buf_size - mv_bytes, tmp, mv_bytes);

		shift_bytes -= mv_bytes;
	}

	shift_bits = positions % CHAR_BIT;

	switch (fill) {
	case eba_fill_zero:
		highest = 0x00;
		break;
	case eba_fill_one:
		highest = 0xFF;
		break;
	case eba_fill_ring:
		highest = buf[0];
		break;
	}
	for (i = 0; i < buf_size; ++i) {
		unsigned char hi_val = buf[i];
		unsigned char lo_val =
		    i < (buf_size - 1) ? buf[i + 1] : highest;
		unsigned u16_a = (((unsigned)hi_val) << 8) | ((unsigned)lo_val);
		unsigned u16_b = ((u16_a << shift_bits) >> 8);
		unsigned char val = 0xFF & ((unsigned char)u16_b);
		buf[i] = val;
	}
}

static void eba_shift_left_el_(struct eba *eba, unsigned long positions,
			       enum eba_shift_fill_val fill)
{
	unsigned char *buf = NULL;
	size_t i = 0;
	size_t buf_size = 0;
	size_t shift_bytes = 0;
	size_t shift_bits = 0;
	size_t size_bits = 0;
	unsigned char highest = 0;
	size_t tmp_size = Eba_stack_buf_size;
	unsigned char tmp[Eba_stack_buf_size];

	eembed_memset(tmp, (fill == eba_fill_one ? 0xFF : 0x00), tmp_size);

	eba_assert_not_null_(eba);
	eembed_assert(CHAR_BIT == 8);
	eembed_assert(eba->size_bytes);
	eembed_assert(eba->size_bytes < (ULONG_MAX * 8));

	size_bits = eba->size_bytes * CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			int all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return;
		}
	}

	if (positions == 0) {
		return;
	}

	buf = eba->bits;
	buf_size = eba->size_bytes;

	shift_bytes = positions / 8;
	while (shift_bytes) {
		size_t mv_bytes = eba_min_(shift_bytes, tmp_size);

		if (fill == eba_fill_ring) {
			unsigned char *orig = buf + (buf_size - mv_bytes);
			eembed_memcpy(tmp, orig, mv_bytes);
		};

		eembed_memmove(buf + mv_bytes, buf, buf_size - mv_bytes);
		eembed_memcpy(buf, tmp, mv_bytes);

		shift_bytes -= mv_bytes;
	}

	shift_bits = positions % CHAR_BIT;

	switch (fill) {
	case eba_fill_zero:
		highest = 0x00;
		break;
	case eba_fill_one:
		highest = 0xFF;
		break;
	case eba_fill_ring:
		highest = buf[buf_size - 1];
		break;
	};
	for (i = 0; i < buf_size; ++i) {
		size_t byte_pos = buf_size - (1 + i);
		unsigned char hi_val = buf[byte_pos];
		unsigned char lo_val = byte_pos ? buf[byte_pos - 1] : highest;
		unsigned u16_a = (((unsigned)hi_val) << 8) | ((unsigned)lo_val);
		unsigned u16_b = ((u16_a << shift_bits) >> 8);
		unsigned char val = 0xFF & ((unsigned char)u16_b);
		buf[byte_pos] = val;
	}

	return;
}

static void eba_shift_left_(struct eba *eba, unsigned long positions,
			    enum eba_shift_fill_val fill)
{
	if (eba->endian == eba_big_endian) {
		eba_shift_left_be_(eba, positions, fill);
	} else {
		eba_shift_left_el_(eba, positions, fill);
	}
}

void eba_rotate_right(struct eba *eba, unsigned long positions)
{
	eba_shift_right_(eba, positions, eba_fill_ring);
}

void eba_rotate_left(struct eba *eba, unsigned long positions)
{
	eba_shift_left_(eba, positions, eba_fill_ring);
}

void eba_shift_left(struct eba *eba, unsigned long positions)
{
	eba_shift_left_(eba, positions, eba_fill_zero);
}

void eba_shift_left_fill(struct eba *eba, unsigned long positions,
			 unsigned char fillval)
{
	eba_shift_left_(eba, positions, fillval ? eba_fill_one : eba_fill_zero);
}

void eba_shift_right(struct eba *eba, unsigned long positions)
{
	eba_shift_right_(eba, positions, eba_fill_zero);
}

void eba_shift_right_fill(struct eba *eba, unsigned long positions,
			  unsigned char fillval)
{
	eba_shift_right_(eba, positions,
			 fillval ? eba_fill_one : eba_fill_zero);
}

#endif /* (!(EBA_SKIP_SHIFTS)) */

#if (!(EBA_SKIP_NEW))

struct eba *eba_new_endian(unsigned long num_bits, enum eba_endian endian)
{
	struct eba *eba = NULL;
	unsigned char *bytes = NULL;
	size_t len = 0;
	size_t eba_s_size = 0;
	size_t array_size = 0;

	array_size = num_bits / CHAR_BIT;
	if ((array_size * CHAR_BIT) < num_bits) {
		array_size += 1;
	}

	eba_s_size = sizeof(struct eba);
	len = eembed_align(eba_s_size) + array_size;
	bytes = (unsigned char *)eembed_malloc(len);
	if (!bytes) {
		return NULL;
	}

	eba = eba_from_bytes(bytes, len, endian);
	eba_set_all(eba, 0x00);
	return eba;
}

struct eba *eba_new(unsigned long num_bits)
{
	return eba_new_endian(num_bits, eba_big_endian);
}

void eba_free(struct eba *eba)
{
	eembed_free(eba);
}
#endif /* #if (!(EBA_SKIP_NEW)) */

static void eba_get_byte_and_offset_(struct eba *eba, unsigned long index,
				     size_t *byte, unsigned char *offset)
{
	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / CHAR_BIT;
	*offset = index % CHAR_BIT;
	eembed_assert((*byte) < eba->size_bytes);

	if (eba->endian == eba_big_endian) {
		*byte = (eba->size_bytes - 1) - (*byte);
	}
}

#if (!(EBA_SKIP_TO_STRING))
char *eba_to_string(struct eba *eba, char *buf, size_t len)
{
	size_t i = 0;
	size_t pos = 0;
	size_t size_bits = 0;
	size_t done = 0;

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

	size_bits = eba->size_bytes * CHAR_BIT;
	pos = 0;
	done = 0;
	if (eba->endian == eba_endian_little) {
		for (i = 0; pos < (len - 1) && i < size_bits; ++i) {
			++done;
			buf[pos++] = eba_get(eba, i) ? '1' : '0';
			if ((done % CHAR_BIT == 0)
			    && (i < (size_bits - 1))
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	} else {
		for (i = size_bits; pos < (len - 1) && i; --i) {
			++done;
			buf[pos++] = eba_get(eba, (i - 1)) ? '1' : '0';
			if ((done % CHAR_BIT == 0) && (i - 1)
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	}

	if ((pos < len) && (done == size_bits)) {
		buf[pos] = '\0';
		return buf;
	}

	buf[len - 2] = '!';
	buf[len - 1] = '\0';
	return buf;
}
#endif /* EBA_SKIP_TO_STRING */
