/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-rotate.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int eba_test_rotate_endian(int verbose, enum eba_endian endian)
{
	int failures;
	unsigned char bytes[10];
	unsigned char start[10];
	unsigned char middle[10];
	struct eba eba;
	unsigned int shift_amount;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_rotate_endian", endian);
	failures = 0;

	eba_memset(bytes, 0x00, 10);
	eba_memset(start, 0x00, 10);
	eba_memset(middle, 0x00, 10);

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = endian;

	start[1] = (1U << 2);
	start[3] = (1U << 0);
	start[8] = ((1U << 0) | (1U << 3) | (1U << 5));
	start[9] = ((1U << 0) | (1U << 4) | (1U << 7));

	shift_amount = 19;
	if (endian == eba_endian_little) {
		middle[0] = ((1U << 3) | (1U << 6));
		middle[1] = ((1U << 0) | (1U << 3) | (1U << 7));
		middle[2] = (1U << 2);
		middle[3] = (1U << 5);
		middle[5] = (1U << 3);
	} else {
		middle[0] = 0x00;
		middle[1] = 0x08;
		middle[2] = 0x00;
		middle[3] = 0x00;
		middle[4] = 0x00;
		middle[5] = 0x01;
		middle[6] = 0x4c;
		middle[7] = 0x88;
		middle[8] = 0x00;
		middle[9] = 0x20;
	}

	eba_memcpy(bytes, start, 10);

	eba_rotate_left(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 10, middle, 10, "first");

	eba_rotate_right(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 10, start, 10, "second");

	if (failures) {
		Test_log_error(failures, "test_rotate");
	}

	return failures;
}

int eba_test_simple_rotate_le(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[2];
	unsigned char start[2];
	unsigned char middle[2];
	struct eba eba;
	unsigned long shift_amount;

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_simple_rotate_le");
	failures = 0;

	for (i = 0; i < 2; ++i) {
		bytes[i] = 0;
		start[i] = 0;
		middle[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 2;
	eba.endian = eba_endian_little;

	start[0] = 5;
	start[1] = 8;

	for (i = 0; i < 2; ++i) {
		bytes[i] = start[i];
	}

	shift_amount = 2;
	eba_rotate_left(&eba, shift_amount);

	middle[0] = (5U << 2);
	middle[1] = (8U << 2);

	failures += check_byte_array_m(bytes, 2, middle, 2, "first");

	eba_rotate_right(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 2, start, 2, "second");

	if (failures) {
		Test_log_error(failures, "test_simple_rotate_le");
	}

	return failures;
}

int eba_test_round_the_world_shift_be(void)
{
	int failures = 0;
	struct eba eba_s;
	unsigned char bytes[2];
	struct eba *eba = &eba_s;
	char buf[40];
	size_t bits_size = 16;

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);
	failures += check_str(buf, "00000001 00101110");
	eba_rotate_left(eba, bits_size + 3);
	eba_rotate_right(eba, bits_size + 5);
	eba_to_string(eba, buf, 40);
	failures += check_str(buf, "10000000 01001011");

	if (failures) {
		Test_log_error(failures, "test_round_the_world_shift_be");
	}
	return failures;
}

int eba_test_round_the_world_shift_el(void)
{
	int failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];
	size_t bits_size = 16;

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_endian_little;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);
	failures += check_str(buf, "01110100 10000000");
	eba_rotate_left(eba, bits_size + 3);
	eba_rotate_right(eba, bits_size + 5);
	eba_to_string(eba, buf, 40);
	failures += check_str(buf, "11010010 00000001");

	if (failures) {
		Test_log_error(failures, "test_round_the_world_shift_el");
	}
	return failures;
}

int eba_test_rotate(int v)
{
	int failures = 0;

	failures += eba_test_rotate_endian(v, eba_big_endian);
	failures += eba_test_rotate_endian(v, eba_endian_little);

	failures += eba_test_simple_rotate_le(v);

	failures += eba_test_round_the_world_shift_be();
	failures += eba_test_round_the_world_shift_el();

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return failures;
}

EBA_TEST(eba_test_rotate)
