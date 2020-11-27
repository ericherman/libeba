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

unsigned char eba_get(struct eba *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;
	eba_assert_not_null_(eba);
	eba_get_byte_and_offset_(eba, index, &byte, &offset);
	return (eba->bits[byte] >> offset) & 0x01;
}

#if (!(EBA_SKIP_SET_ALL))
void eba_set_all(struct eba *eba, unsigned char val)
{
	int all_vals;
	eba_assert_not_null_(eba);
	all_vals = val ? UCHAR_MAX : 0;
	eembed_memset(eba->bits, all_vals, eba->size_bytes);
}
#endif /* (!(EBA_SKIP_SET_ALL)) */

#if (!(EBA_SKIP_TOGGLE))
void eba_toggle(struct eba *eba, unsigned long index)
{
	size_t byte;
	unsigned char offset;
	eba_assert_not_null_(eba);
	eba_get_byte_and_offset_(eba, index, &byte, &offset);
	eba->bits[byte] ^= (1U << offset);
}
#endif /* (!(EBA_SKIP_TOGGLE)) */

#if (!(EBA_SKIP_SWAP))
void eba_swap(struct eba *eba, unsigned long index1, unsigned long index2)
{
	unsigned char tmp1, tmp2;
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

static void eba_reverse_bytes_(unsigned char *bytes, size_t len)
{
	size_t i, j;
	unsigned char swap;
	eembed_assert(bytes);
	eembed_assert(len);
	for (i = 0, j = len - 1; i < j; ++i, --j) {
		swap = bytes[i];
		bytes[i] = bytes[j];
		bytes[j] = swap;
	}
}

static int eba_shift_right_be_(struct eba *eba, unsigned long positions,
			       enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos, len;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba *tmp;
	if (positions == 0) {
		return 0;
	}

	size_bits = eba->size_bytes * EEMBED_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(eembed_alloca(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eembed_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eembed_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eembed_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	eembed_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);
	if (EBA_DEBUG) {
		eembed_memset(eba->bits, 0, eba->size_bytes);
	}
	shift_bytes = positions / EEMBED_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) <<
					  EEMBED_CHAR_BIT);
		u16_b = (u16_a >> (positions % EEMBED_CHAR_BIT));
		val = UCHAR_MAX & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	eembed_freea(tmp);
	return 0;
}

static int eba_shift_right_el_(struct eba *eba, unsigned long positions,
			       enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos, len;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba *tmp;
	if (positions == 0) {
		return 0;
	}

	size_bits = eba->size_bytes * EEMBED_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(eembed_alloca(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eembed_memset(tmp->bits + eba->size_bytes, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eembed_memset(tmp->bits + eba->size_bytes, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eembed_memcpy(tmp->bits +
			      eba->size_bytes, eba->bits, eba->size_bytes);
		break;
	};
	eembed_memcpy(tmp->bits, eba->bits, eba->size_bytes);
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);
	if (EBA_DEBUG) {
		eembed_memset(eba->bits, 0, eba->size_bytes);
	}
	shift_bytes = positions / EEMBED_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) <<
					  EEMBED_CHAR_BIT);
		u16_b = (u16_a >> (positions % EEMBED_CHAR_BIT));
		val = UCHAR_MAX & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	eba_reverse_bytes_(eba->bits, eba->size_bytes);
	eembed_freea(tmp);
	return 0;
}

static int eba_shift_right_(struct eba *eba, unsigned long positions,
			    enum eba_shift_fill_val fill)
{
	if (eba->endian == eba_big_endian) {
		return eba_shift_right_be_(eba, positions, fill);
	} else {
		return eba_shift_right_el_(eba, positions, fill);
	}
}

static int eba_shift_left_be_(struct eba *eba, unsigned long positions,
			      enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos, len;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba *tmp;
	if (positions == 0) {
		return 0;
	}

	size_bits = eba->size_bytes * EEMBED_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(eembed_alloca(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eembed_memset(tmp->bits + eba->size_bytes, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eembed_memset(tmp->bits + eba->size_bytes, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eembed_memcpy(tmp->bits +
			      eba->size_bytes, eba->bits, eba->size_bytes);
		break;
	};
	eembed_memcpy(tmp->bits, eba->bits, eba->size_bytes);
	if (EBA_DEBUG) {
		eembed_memset(eba->bits, 0, eba->size_bytes);
	}
	shift_bytes = positions / EEMBED_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) <<
					  EEMBED_CHAR_BIT);
		u16_b = (u16_a << (positions % EEMBED_CHAR_BIT));
		val = ((unsigned char)(u16_b >> EEMBED_CHAR_BIT));
		eba->bits[i] = val;
	}

	eembed_freea(tmp);
	return 0;
}

static int eba_shift_left_el_(struct eba *eba, unsigned long positions,
			      enum eba_shift_fill_val fill)
{
	unsigned long i, size_bits;
	size_t shift_bytes, byte_pos, len;
	unsigned char val, hi_val, lo_val;
	int all_vals;
	unsigned int u16_a, u16_b;
	struct eba *tmp;
	if (positions == 0) {
		return 0;
	}

	size_bits = eba->size_bytes * EEMBED_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eembed_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(eembed_alloca(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eembed_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eembed_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eembed_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	eembed_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);
	eembed_memset(eba->bits, 0, eba->size_bytes);
	shift_bytes = positions / EEMBED_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) <<
					  EEMBED_CHAR_BIT);
		u16_b = (u16_a << (positions % EEMBED_CHAR_BIT));
		val = ((unsigned char)(u16_b >> EEMBED_CHAR_BIT));
		eba->bits[i] = val;
	}

	eba_reverse_bytes_(eba->bits, eba->size_bytes);
	eembed_freea(tmp);
	return 0;
}

static int eba_shift_left_(struct eba *eba, unsigned long positions,
			   enum eba_shift_fill_val fill)
{
	if (eba->endian == eba_big_endian) {
		return eba_shift_left_be_(eba, positions, fill);
	} else {
		return eba_shift_left_el_(eba, positions, fill);
	}
}

int eba_rotate_right(struct eba *eba, unsigned long positions)
{
	return eba_shift_right_(eba, positions, eba_fill_ring);
}

int eba_rotate_left(struct eba *eba, unsigned long positions)
{
	return eba_shift_left_(eba, positions, eba_fill_ring);
}

int eba_shift_left(struct eba *eba, unsigned long positions)
{
	return eba_shift_left_(eba, positions, eba_fill_zero);
}

int eba_shift_left_fill(struct eba *eba, unsigned long positions,
			unsigned char fillval)
{
	return eba_shift_left_(eba, positions,
			       fillval ? eba_fill_one : eba_fill_zero);
}

int eba_shift_right(struct eba *eba, unsigned long positions)
{
	return eba_shift_right_(eba, positions, eba_fill_zero);
}

int eba_shift_right_fill(struct eba *eba, unsigned long positions,
			 unsigned char fillval)
{
	return eba_shift_right_(eba, positions,
				fillval ? eba_fill_one : eba_fill_zero);
}

#endif /* (!(EBA_SKIP_SHIFTS)) */

#if (!(EBA_SKIP_NEW))

struct eba *eba_new_endian(unsigned long num_bits, enum eba_endian endian)
{
	struct eba *eba;
	unsigned char *bytes;
	size_t len;
	size_t eba_s_size;
	size_t array_size;

	array_size = num_bits / EEMBED_CHAR_BIT;
	if ((array_size * EEMBED_CHAR_BIT) < num_bits) {
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
	*byte = index / EEMBED_CHAR_BIT;
	*offset = index % EEMBED_CHAR_BIT;
	eembed_assert((*byte) < eba->size_bytes);

	if (eba->endian == eba_big_endian) {
		*byte = (eba->size_bytes - 1) - (*byte);
	}
}

#if (!(EBA_SKIP_TO_STRING))
char *eba_to_string(struct eba *eba, char *buf, size_t len)
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

	size_bits = eba->size_bytes * EEMBED_CHAR_BIT;
	pos = 0;
	done = 0;
	if (eba->endian == eba_endian_little) {
		for (i = 0; pos < (len - 1) && i < size_bits; ++i) {
			++done;
			buf[pos++] = eba_get(eba, i) ? '1' : '0';
			if ((done % EEMBED_CHAR_BIT == 0)
			    && (i < (size_bits - 1))
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	} else {
		for (i = size_bits; pos < (len - 1) && i; --i) {
			++done;
			buf[pos++] = eba_get(eba, (i - 1)) ? '1' : '0';
			if ((done % EEMBED_CHAR_BIT == 0) && (i - 1)
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
