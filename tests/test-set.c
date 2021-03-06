/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

unsigned eba_test_set_endian(int verbose, enum eba_endian endian)
{
	unsigned failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba eba;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_set_endian", endian);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		expected[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = endian;

	if (endian == eba_big_endian) {
		expected[9] = 4;
		expected[8] = 6;
		expected[3] = 255;
		expected[1] = 251;
	} else {
		expected[0] = 4;
		expected[1] = 6;
		expected[6] = 255;
		expected[8] = 251;
	}

	/* test that "20" becomes "1" */
	eba_set(&eba, 2, 20);

	/* test indexing into second byte, twice */
	eba_set(&eba, 9, 1);
	eba_set(&eba, 10, 1);

	/* test setting each bit of a byte */
	eba_set(&eba, 48, 1);
	eba_set(&eba, 49, 1);
	eba_set(&eba, 50, 1);
	eba_set(&eba, 51, 1);
	eba_set(&eba, 52, 1);
	eba_set(&eba, 53, 1);
	eba_set(&eba, 54, 1);
	eba_set(&eba, 55, 1);

	/* test setting each bit of a byte, then set one bit back to zero */
	eba_set(&eba, 64, 1);
	eba_set(&eba, 65, 1);
	eba_set(&eba, 66, 1);
	eba_set(&eba, 67, 1);
	eba_set(&eba, 68, 1);
	eba_set(&eba, 69, 1);
	eba_set(&eba, 70, 1);
	eba_set(&eba, 71, 1);
	eba_set(&eba, 66, 0);

	failures += check_byte_array(bytes, 10, expected, 10);

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_set(int v)
{
	unsigned failures = 0;

	failures += eba_test_set_endian(v, eba_big_endian);
	failures += eba_test_set_endian(v, eba_endian_little);

	return failures;
}

ECHECK_TEST_MAIN_V(eba_test_set)
