/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-swap.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int eba_test_swap_endianness(int verbose, enum eba_endian endian)
{
	int failures;
	size_t i, j;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba eba;
	unsigned int index1, index2;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_swap_endianness", endian);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		expected[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = endian;

	expected[1] = 251;
	expected[3] = 255;
	expected[8] = 6;
	expected[9] = 4;

	bytes[0] = 4;
	bytes[1] = 6;
	bytes[6] = 255;
	bytes[8] = 251;

	for (i = 0; i < 5; ++i) {
		for (j = 0; j < 8; ++j) {
			index1 = (i * 8) + j;
			index2 = ((9 * 8) - (i * 8)) + j;
			eba_swap(&eba, index1, index2);
		}
	}

	failures += check_byte_array(bytes, 10, expected, 10);

	if (failures) {
		Test_log_error(failures, "test_swap_endianness");
	}

	return failures;
}

int eba_test_swap(int v)
{
	int failures = 0;

	failures += eba_test_swap_endianness(v, eba_endian_little);
	failures += eba_test_swap_endianness(v, eba_big_endian);

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return failures;
}

EBA_TEST(eba_test_swap)
