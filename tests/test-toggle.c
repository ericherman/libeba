/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-toggle.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"
#include <limits.h>

int eba_test_toggle_endian(int verbose, enum eba_endian endian)
{
	int failures = 0;
	size_t i = 0;
	unsigned char bytes[2];
	unsigned char expected[2];
	struct eba eba;

	eba.bits = bytes;
	eba.size_bytes = 2;
	eba.endian = endian;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_toggle_endian", endian);

	for (i = 0; i < eba.size_bytes; ++i) {
		eba.bits[i] = 0;
	}

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 0);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 0);
	failures += check_int(eba_get(&eba, 10), 0);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	eba_toggle(&eba, 7);

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 1);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 0);
	failures += check_int(eba_get(&eba, 10), 0);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	eba_toggle(&eba, 9);

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 1);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 0);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	eba_toggle(&eba, 10);

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 1);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 1);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);
	failures += check_int(eba_get(&eba, 13), 0);

	eba_toggle(&eba, 13);

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 1);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 1);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 1);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	eba_toggle(&eba, 13);

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 0);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 1);
	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 1);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	expected[(endian == eba_big_endian) ? 1 : 0] = (1U << 7);
	expected[(endian == eba_big_endian) ? 0 : 1] =
	    ((1U << (9 - CHAR_BIT)) | (1U << (10 - CHAR_BIT)));
	failures += check_byte_array(bytes, 2, expected, 2);

	if (failures) {
		Test_log_error(failures, "test_toggle");
	}

	return failures;
}

int eba_test_toggle_all_el(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba eba;

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_toggle_all_el");
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		expected[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = eba_endian_little;

	expected[0] = 4;
	expected[1] = 6;
	expected[6] = 255;
	expected[8] = 251;

	for (i = 0; i < 10; ++i) {
		bytes[i] = ~(expected[i]);
	}

	for (i = 0; i < 80; ++i) {
		eba_toggle(&eba, i);
	}
	failures += check_byte_array(bytes, 10, expected, 10);

	if (failures) {
		Test_log_error(failures, "test_toggle_all_el");
	}

	return failures;
}

int eba_test_toggle(int v)
{
	int failures = 0;

	failures += eba_test_toggle_endian(v, eba_big_endian);
	failures += eba_test_toggle_endian(v, eba_endian_little);
	failures += eba_test_toggle_all_el(v);

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return failures;
}

EBA_TEST(eba_test_toggle)
