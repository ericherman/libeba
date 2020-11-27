/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set-all.c */
/* Copyright (C) 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

unsigned eba_test_set_all_endian(int verbose, enum eba_endian endian)
{
	unsigned failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba eba;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_set_all_endian", endian);
	failures = 0;

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = endian;

	eba_set_all(&eba, 0x00);
	for (i = 0; i < 10; ++i) {
		expected[i] = 0;
	}
	failures += check_byte_array(bytes, 10, expected, 10);

	eba_set_all(&eba, 0x07);
	for (i = 0; i < 10; ++i) {
		expected[i] = 0xFF;
	}
	failures += check_byte_array(bytes, 10, expected, 10);

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_set_all(int v)
{
	unsigned failures = 0;

	failures += eba_test_set_all_endian(v, eba_big_endian);
	failures += eba_test_set_all_endian(v, eba_endian_little);

	return failures;
}

ECHECK_TEST_MAIN_V(eba_test_set_all)
