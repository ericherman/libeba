/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set-all.c */
/* Copyright (C) 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int test_set_all(int verbose, enum eba_endian endian)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
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

	if (failures) {
		Test_log_error1("%d failures in test_set\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_set_all(v, eba_big_endian);

#if (!(EBA_SKIP_ENDIAN))
	failures += test_set_all(v, eba_endian_little);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
