/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba.c: embedable bit array - hopefully somewhat suitable for small CPUs */
/* Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba.h"

/* Some embedded build systems make it easy to add #define to the compile
 * step, but Arduino's build system is very rigid by default, so we make
 * some simplifying assumptions. If debug is needed, then this will need
 * to be edited in the source here. */
#ifdef ARDUINO

#ifndef EBA_HOSTED
#define EBA_HOSTED 0
#endif

#ifndef EBA_DEBUG
#define EBA_DEBUG 0
#endif

#endif /* ARDUINO */

#ifndef EBA_HOSTED
/*
 __STDC_HOSTED__
 The integer constant 1 if the implementation is a hosted
 implementation or the integer constant 0 if it is not.

 C99 standard (section 6.10.8):
 http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf

 C++11 standard (section 16.8):
 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf

 "The standard also defines two environments for programs, a
 freestanding environment, required of all implementations and which
 may not have library facilities beyond those required of
 freestanding implementations, where the handling of program startup
 and termination are implementation-defined; and a hosted
 environment, which is not required, in which all the library
 facilities are provided and startup is through a function int main
 (void) or int main (int, char *[]). An OS kernel is an example of a
 program running in a freestanding environment; a program using the
 facilities of an operating system is an example of a program
 running in a hosted environment."
 https://gcc.gnu.org/onlinedocs/gcc/Standards.html
*/
#ifdef __STDC_HOSTED__
#define EBA_HOSTED __STDC_HOSTED__
#else
/* guess? */
#define EBA_HOSTED 1
#endif
#endif

#ifndef EBA_DEBUG
#ifdef NDEBUG
#define EBA_DEBUG 0
#else
#define EBA_DEBUG 1
#endif
#endif

#define EBA_NOP() do { ((void)0); } while (0)

#ifndef eba_assert
#if EBA_HOSTED
#include <assert.h>
#define eba_assert(expression) \
	assert(expression)
#else
#define eba_assert(expression) \
	do { \
		if (expression) { \
			EBA_NOP(); \
		} else { \
			eba_debug_print_s(__FILE__); \
			eba_debug_print_s(":"); \
			eba_debug_print_z(__LINE__); \
			eba_debug_print_s(": ASSERTION assert("); \
			eba_debug_print_s(#expression); \
			eba_debug_print_s(") FAILED"); \
			eba_debug_print_eol(); \
			eba_debug_die(); \
		} \
	} while (0)
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

/**********************************************************************/
/* internal (stack) allocation functions */
/**********************************************************************/
/*
 * In the shifting code, I would like to simly write something like:
 * "unsigned char bytes[foo];" where "foo" is a function parameter
 * But this is not widely supported.
 *
 * While C99,C11 have variable stack allocation, C++ and C89 do not.
 * As it happens, gcc supports alloca even for 8bit CPUs.
 * I would like wider-spread support of more modern C99/C11 especially
 * from Visual C++, but for now I'll just do this.
 *
 * This whole Eba_scope_alloc idea should be replaced with something
 * better, but I am out of ideas at the moment.
 */
#if (EBA_NO_ALLOCA || __STDC_NO_VLA__)
#define Eba_scope_alloc(len) eba_alloc(eba_alloc_context, len)
#define Eba_scope_free(p) eba_mfree(eba_alloc_context, p)
#else /* (EBA_NO_ALLOCA || __STDC_NO_VLA__) */
#if (!(EBA_SKIP_ALLOCA_H))
#include <alloca.h>
#endif /* (!(EBA_SKIP_ALLOCA_H)) */
#define Eba_scope_alloc(len) alloca(len)
#define Eba_scope_free(p) EBA_NOP()
#endif /* (EBA_NO_ALLOCA || __STDC_NO_VLA__) */

/**********************************************************************/
/* bits in a byte */
/**********************************************************************/
/*
 * C defines a char to be at least 8 bits,
 * but a char *may* be more ...
 * thus use CHAR_BIT from limits.h
 * unless compiled with -DEBA_CHAR_BIT=8 or similar
 * POSIX insists that CHAR_BIT is 8, and all modern OSes this is true
 * of ... however the embedded space is not 100% uniform. Searching
 * for "#define CHARBIT 16" and "CHARBIT 32" has a few hits ....
 */
#ifndef EBA_CHAR_BIT
#ifdef CHAR_BIT
#define EBA_CHAR_BIT CHAR_BIT
#else
#include <limits.h>
#ifdef CHAR_BIT
#define EBA_CHAR_BIT CHAR_BIT
#endif
#endif
#endif

#ifndef EBA_CHAR_BIT
#define EBA_CHAR_BIT 8
#endif

void *eba_alloc_context = NULL;

#if EBA_HOSTED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void *(*eba_memcpy)(void *dest, const void *src, size_t n) = memcpy;
void *(*eba_memset)(void *s, int c, size_t n) = memset;
void *eba_libc_malloc(void *context, size_t size)
{
	(void)context;
	return malloc(size);
}

void *(*eba_alloc)(void *context, size_t size) = eba_libc_malloc;
void eba_libc_free(void *context, void *ptr)
{
	(void)context;
	free(ptr);
}

void (*eba_mfree)(void *context, void *ptr) = eba_libc_free;

void eba_stderr_print_z(size_t z)
{
	fprintf(stderr, "%lu", (unsigned long)z);
}

void (*eba_debug_print_z)(size_t z) = eba_stderr_print_z;

void eba_stderr_print_s(const char *s)
{
	fprintf(stderr, "%s", s);
}

void (*eba_debug_print_s)(const char *s) = eba_stderr_print_s;

void eba_stderr_print_eol(void)
{
	fprintf(stderr, "\n");
}

void (*eba_debug_print_eol)(void) = eba_stderr_print_eol;

void eba_exit_failure(void)
{
	exit(EXIT_FAILURE);
}

void (*eba_debug_die)(void) = eba_exit_failure;

#else /* EBA_HOSTED */

#if EBA_SKIP_DIY_MEMCPY
void *(*eba_memcpy)(void *dest, const void *src, size_t n) = NULL;
#else
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

void *(*eba_memcpy)(void *dest, const void *src, size_t n) = eba_diy_memcpy;
#endif

#if EBA_SKIP_DIY_MEMSET
void *(*eba_memset)(void *s, int c, size_t n) = NULL;
#else
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

void *(*eba_memset)(void *s, int c, size_t n) = eba_diy_memset;
#endif

#if EBA_SKIP_NO_MALLOC
void *(*eba_alloc)(void *context, size_t size) = NULL;
void (*eba_mfree)(void *context, void *ptr) = NULL;
#else
void *eba_no_malloc(void *context, size_t size)
{
	(void)context;
	(void)size;
	return NULL;
}

void *(*eba_alloc)(void *context, size_t size) = eba_no_malloc;
void eba_no_free(void *context, void *ptr)
{
	(void)context;
	(void)ptr;
}

void (*eba_mfree)(void *context, void *ptr) = eba_no_free;
#endif /* EBA_SKIP_NO_MALLOC */

#if EBA_SKIP_NO_PRINT
void (*eba_debug_print_z)(size_t z) = NULL;
void (*eba_debug_print_s)(const char *s) = NULL;
void (*eba_debug_print_eol)(void) = NULL;
#else
void eba_no_print_z(size_t z)
{
	(void)z;
}

void (*eba_debug_print_z)(size_t z) = eba_no_print_z;

void eba_no_print_s(const char *s)
{
	(void)s;
}

void (*eba_debug_print_s)(const char *s) = eba_no_print_s;

void eba_no_print_eol(void)
{
}

void (*eba_debug_print_eol)(void) = eba_no_print_eol;
#endif /* EBA_SKIP_NO_PRINT */

void (*eba_debug_die)(void) = NULL;
#endif /* EBA_HOSTED */

#if (EBA_DEBUG)
static void eba_assert_not_null_(struct eba *eba)
{
	eba_assert(eba);
	eba_assert(eba->bits);
	eba_assert(eba->size_bytes);
}
#else
#define eba_assert_not_null_(eba) EBA_NOP()
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
	eba_memset(eba->bits, all_vals, eba->size_bytes);
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
	eba_assert(bytes);
	eba_assert(len);
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

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eba_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(Eba_scope_alloc(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eba_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eba_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	eba_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);
	/* eba_memset(eba->bits, 0, eba->size_bytes); */
	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) << EBA_CHAR_BIT);
		u16_b = (u16_a >> (positions % EBA_CHAR_BIT));
		val = UCHAR_MAX & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	Eba_scope_free(tmp);
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

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eba_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(Eba_scope_alloc(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eba_memset(tmp->bits + eba->size_bytes, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eba_memset(tmp->bits + eba->size_bytes, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eba_memcpy(tmp->bits +
			   eba->size_bytes, eba->bits, eba->size_bytes);
		break;
	};
	eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);
	/* eba_memset(eba->bits, 0, eba->size_bytes); */
	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = eba->size_bytes; i; --i) {
		byte_pos = eba->size_bytes + (i - 1) - shift_bytes;
		lo_val = tmp->bits[byte_pos];
		byte_pos -= 1;
		hi_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) << EBA_CHAR_BIT);
		u16_b = (u16_a >> (positions % EBA_CHAR_BIT));
		val = UCHAR_MAX & ((unsigned char)u16_b);
		eba->bits[i - 1] = val;
	}

	eba_reverse_bytes_(eba->bits, eba->size_bytes);
	Eba_scope_free(tmp);
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

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eba_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(Eba_scope_alloc(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eba_memset(tmp->bits + eba->size_bytes, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eba_memset(tmp->bits + eba->size_bytes, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eba_memcpy(tmp->bits +
			   eba->size_bytes, eba->bits, eba->size_bytes);
		break;
	};
	eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
	/* eba_memset(eba->bits, 0, eba->size_bytes); */
	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) << EBA_CHAR_BIT);
		u16_b = (u16_a << (positions % EBA_CHAR_BIT));
		val = ((unsigned char)(u16_b >> EBA_CHAR_BIT));
		eba->bits[i] = val;
	}

	Eba_scope_free(tmp);
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

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	if (positions >= size_bits) {
		if (fill == eba_fill_ring) {
			positions = positions % size_bits;
		} else {
			all_vals = (fill == eba_fill_zero) ? 0 : -1;
			eba_memset(eba->bits, all_vals, eba->size_bytes);
			return 0;
		}
	}

	len = sizeof(struct eba) + (2 * eba->size_bytes);
	tmp = eba_from_bytes(Eba_scope_alloc(len), len, eba->endian);
	if (!tmp) {
		return 1;
	}
	switch (fill) {
	case eba_fill_zero:
		eba_memset(tmp->bits, 0, eba->size_bytes);
		break;
	case eba_fill_one:
		eba_memset(tmp->bits, -1, eba->size_bytes);
		break;
	case eba_fill_ring:
		eba_memcpy(tmp->bits, eba->bits, eba->size_bytes);
		break;
	};
	eba_memcpy(tmp->bits + eba->size_bytes, eba->bits, eba->size_bytes);
	eba_reverse_bytes_(tmp->bits, tmp->size_bytes);
	eba_memset(eba->bits, 0, eba->size_bytes);
	shift_bytes = positions / EBA_CHAR_BIT;
	for (i = 0; i < eba->size_bytes; ++i) {
		byte_pos = i + shift_bytes;
		hi_val = tmp->bits[byte_pos];
		byte_pos += 1;
		lo_val = tmp->bits[byte_pos];
		u16_a =
		    ((unsigned)lo_val) | (((unsigned)hi_val) << EBA_CHAR_BIT);
		u16_b = (u16_a << (positions % EBA_CHAR_BIT));
		val = ((unsigned char)(u16_b >> EBA_CHAR_BIT));
		eba->bits[i] = val;
	}

	eba_reverse_bytes_(eba->bits, eba->size_bytes);
	Eba_scope_free(tmp);
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

	array_size = num_bits / EBA_CHAR_BIT;
	if ((array_size * EBA_CHAR_BIT) < num_bits) {
		array_size += 1;
	}

	eba_s_size = sizeof(struct eba);
	len = eba_s_size + array_size;
	bytes = (unsigned char *)eba_alloc(eba_alloc_context, len);
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
	if (!eba) {
		return;
	}
	eba_mfree(eba_alloc_context, eba);
}
#endif /* #if (!(EBA_SKIP_NEW)) */

static void eba_get_byte_and_offset_(struct eba *eba, unsigned long index,
				     size_t *byte, unsigned char *offset)
{
	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / EBA_CHAR_BIT;
	*offset = index % EBA_CHAR_BIT;
	eba_assert((*byte) < eba->size_bytes);

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

	size_bits = eba->size_bytes * EBA_CHAR_BIT;
	pos = 0;
	done = 0;
	if (eba->endian == eba_endian_little) {
		for (i = 0; pos < (len - 1) && i < size_bits; ++i) {
			++done;
			buf[pos++] = eba_get(eba, i) ? '1' : '0';
			if ((done % EBA_CHAR_BIT == 0)
			    && (i < (size_bits - 1))
			    && (pos < (len - 1))) {
				buf[pos++] = ' ';
			}
		}
	} else {
		for (i = size_bits; pos < (len - 1) && i; --i) {
			++done;
			buf[pos++] = eba_get(eba, (i - 1)) ? '1' : '0';
			if ((done % EBA_CHAR_BIT == 0) && (i - 1)
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
