/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-get.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int test_get_el(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = eba_endian_little;

	bytes[0] = 4;
	bytes[1] = 6;
	bytes[6] = 255;
	bytes[8] = 251;

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 1);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 0);

	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 1);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	failures += check_int(eba_get(&eba, 16), 0);
	failures += check_int(eba_get(&eba, 17), 0);
	failures += check_int(eba_get(&eba, 18), 0);
	failures += check_int(eba_get(&eba, 19), 0);
	failures += check_int(eba_get(&eba, 20), 0);
	failures += check_int(eba_get(&eba, 21), 0);
	failures += check_int(eba_get(&eba, 22), 0);
	failures += check_int(eba_get(&eba, 23), 0);

	failures += check_int(eba_get(&eba, 24), 0);
	failures += check_int(eba_get(&eba, 25), 0);
	failures += check_int(eba_get(&eba, 26), 0);
	failures += check_int(eba_get(&eba, 27), 0);
	failures += check_int(eba_get(&eba, 28), 0);
	failures += check_int(eba_get(&eba, 29), 0);
	failures += check_int(eba_get(&eba, 30), 0);
	failures += check_int(eba_get(&eba, 31), 0);

	failures += check_int(eba_get(&eba, 32), 0);
	failures += check_int(eba_get(&eba, 33), 0);
	failures += check_int(eba_get(&eba, 34), 0);
	failures += check_int(eba_get(&eba, 35), 0);
	failures += check_int(eba_get(&eba, 36), 0);
	failures += check_int(eba_get(&eba, 37), 0);
	failures += check_int(eba_get(&eba, 38), 0);
	failures += check_int(eba_get(&eba, 39), 0);

	failures += check_int(eba_get(&eba, 40), 0);
	failures += check_int(eba_get(&eba, 41), 0);
	failures += check_int(eba_get(&eba, 42), 0);
	failures += check_int(eba_get(&eba, 43), 0);
	failures += check_int(eba_get(&eba, 44), 0);
	failures += check_int(eba_get(&eba, 45), 0);
	failures += check_int(eba_get(&eba, 46), 0);
	failures += check_int(eba_get(&eba, 47), 0);

	failures += check_int(eba_get(&eba, 48), 1);
	failures += check_int(eba_get(&eba, 49), 1);
	failures += check_int(eba_get(&eba, 50), 1);
	failures += check_int(eba_get(&eba, 51), 1);
	failures += check_int(eba_get(&eba, 52), 1);
	failures += check_int(eba_get(&eba, 53), 1);
	failures += check_int(eba_get(&eba, 54), 1);
	failures += check_int(eba_get(&eba, 55), 1);

	failures += check_int(eba_get(&eba, 56), 0);
	failures += check_int(eba_get(&eba, 57), 0);
	failures += check_int(eba_get(&eba, 58), 0);
	failures += check_int(eba_get(&eba, 59), 0);
	failures += check_int(eba_get(&eba, 60), 0);
	failures += check_int(eba_get(&eba, 61), 0);
	failures += check_int(eba_get(&eba, 62), 0);
	failures += check_int(eba_get(&eba, 63), 0);

	failures += check_int(eba_get(&eba, 64), 1);
	failures += check_int(eba_get(&eba, 65), 1);
	failures += check_int(eba_get(&eba, 66), 0);
	failures += check_int(eba_get(&eba, 67), 1);
	failures += check_int(eba_get(&eba, 68), 1);
	failures += check_int(eba_get(&eba, 69), 1);
	failures += check_int(eba_get(&eba, 70), 1);
	failures += check_int(eba_get(&eba, 71), 1);

	failures += check_int(eba_get(&eba, 72), 0);
	failures += check_int(eba_get(&eba, 73), 0);
	failures += check_int(eba_get(&eba, 74), 0);
	failures += check_int(eba_get(&eba, 75), 0);
	failures += check_int(eba_get(&eba, 76), 0);
	failures += check_int(eba_get(&eba, 77), 0);
	failures += check_int(eba_get(&eba, 78), 0);
	failures += check_int(eba_get(&eba, 79), 0);

	if (failures) {
		Test_log_error1("%d failures in test_get\n", failures);
	}

	return failures;
}

int test_get_be(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = eba_big_endian;

	bytes[9] = 4;
	bytes[8] = 6;
	bytes[3] = 255;
	bytes[1] = 251;

	failures += check_int(eba_get(&eba, 0), 0);
	failures += check_int(eba_get(&eba, 1), 0);
	failures += check_int(eba_get(&eba, 2), 1);
	failures += check_int(eba_get(&eba, 3), 0);
	failures += check_int(eba_get(&eba, 4), 0);
	failures += check_int(eba_get(&eba, 5), 0);
	failures += check_int(eba_get(&eba, 6), 0);
	failures += check_int(eba_get(&eba, 7), 0);

	failures += check_int(eba_get(&eba, 8), 0);
	failures += check_int(eba_get(&eba, 9), 1);
	failures += check_int(eba_get(&eba, 10), 1);
	failures += check_int(eba_get(&eba, 11), 0);
	failures += check_int(eba_get(&eba, 12), 0);
	failures += check_int(eba_get(&eba, 13), 0);
	failures += check_int(eba_get(&eba, 14), 0);
	failures += check_int(eba_get(&eba, 15), 0);

	failures += check_int(eba_get(&eba, 16), 0);
	failures += check_int(eba_get(&eba, 17), 0);
	failures += check_int(eba_get(&eba, 18), 0);
	failures += check_int(eba_get(&eba, 19), 0);
	failures += check_int(eba_get(&eba, 20), 0);
	failures += check_int(eba_get(&eba, 21), 0);
	failures += check_int(eba_get(&eba, 22), 0);
	failures += check_int(eba_get(&eba, 23), 0);

	failures += check_int(eba_get(&eba, 24), 0);
	failures += check_int(eba_get(&eba, 25), 0);
	failures += check_int(eba_get(&eba, 26), 0);
	failures += check_int(eba_get(&eba, 27), 0);
	failures += check_int(eba_get(&eba, 28), 0);
	failures += check_int(eba_get(&eba, 29), 0);
	failures += check_int(eba_get(&eba, 30), 0);
	failures += check_int(eba_get(&eba, 31), 0);

	failures += check_int(eba_get(&eba, 32), 0);
	failures += check_int(eba_get(&eba, 33), 0);
	failures += check_int(eba_get(&eba, 34), 0);
	failures += check_int(eba_get(&eba, 35), 0);
	failures += check_int(eba_get(&eba, 36), 0);
	failures += check_int(eba_get(&eba, 37), 0);
	failures += check_int(eba_get(&eba, 38), 0);
	failures += check_int(eba_get(&eba, 39), 0);

	failures += check_int(eba_get(&eba, 40), 0);
	failures += check_int(eba_get(&eba, 41), 0);
	failures += check_int(eba_get(&eba, 42), 0);
	failures += check_int(eba_get(&eba, 43), 0);
	failures += check_int(eba_get(&eba, 44), 0);
	failures += check_int(eba_get(&eba, 45), 0);
	failures += check_int(eba_get(&eba, 46), 0);
	failures += check_int(eba_get(&eba, 47), 0);

	failures += check_int(eba_get(&eba, 48), 1);
	failures += check_int(eba_get(&eba, 49), 1);
	failures += check_int(eba_get(&eba, 50), 1);
	failures += check_int(eba_get(&eba, 51), 1);
	failures += check_int(eba_get(&eba, 52), 1);
	failures += check_int(eba_get(&eba, 53), 1);
	failures += check_int(eba_get(&eba, 54), 1);
	failures += check_int(eba_get(&eba, 55), 1);

	failures += check_int(eba_get(&eba, 56), 0);
	failures += check_int(eba_get(&eba, 57), 0);
	failures += check_int(eba_get(&eba, 58), 0);
	failures += check_int(eba_get(&eba, 59), 0);
	failures += check_int(eba_get(&eba, 60), 0);
	failures += check_int(eba_get(&eba, 61), 0);
	failures += check_int(eba_get(&eba, 62), 0);
	failures += check_int(eba_get(&eba, 63), 0);

	failures += check_int(eba_get(&eba, 64), 1);
	failures += check_int(eba_get(&eba, 65), 1);
	failures += check_int(eba_get(&eba, 66), 0);
	failures += check_int(eba_get(&eba, 67), 1);
	failures += check_int(eba_get(&eba, 68), 1);
	failures += check_int(eba_get(&eba, 69), 1);
	failures += check_int(eba_get(&eba, 70), 1);
	failures += check_int(eba_get(&eba, 71), 1);

	failures += check_int(eba_get(&eba, 72), 0);
	failures += check_int(eba_get(&eba, 73), 0);
	failures += check_int(eba_get(&eba, 74), 0);
	failures += check_int(eba_get(&eba, 75), 0);
	failures += check_int(eba_get(&eba, 76), 0);
	failures += check_int(eba_get(&eba, 77), 0);
	failures += check_int(eba_get(&eba, 78), 0);
	failures += check_int(eba_get(&eba, 79), 0);

	if (failures) {
		Test_log_error1("%d failures in test_get\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_get_be(v);
#if Eba_need_endian
	failures += test_get_el(v);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	fprintf(stderr, "falures: %d(%d)\n", failures, cap_failures(failures));
	return cap_failures(failures);
}
