/*
test-toggle.c
Copyright (C) 2017 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/
#include "eba-test-private-utils.h"

int test_toggle(int verbose, enum eba_endian endian)
{
	int failures;
	size_t i;
	unsigned char bytes[2];
	unsigned char expected[2];
	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 2; ++i) {
		bytes[i] = 0;
	}

	eba.bits = bytes;
	eba.size = 2;
	eba.endian = endian;

	expected[(endian == eba_big_endian) ? 1 : 0] = (1U << 7);
	expected[(endian == eba_big_endian) ? 0 : 1] =
	    ((1U << (9 - 8)) | (1U << (10 - 8)));

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

	failures += check_byte_array(bytes, 2, expected, 2);

	if (failures) {
		Test_log_error1("%d failures in test_toggle2\n", failures);
	}

	return failures;
}

int test_toggle_all(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		expected[i] = 0;
	}

	eba.bits = bytes;
	eba.size = 10;
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
		Test_log_error1("%d failures in test_toggle\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_toggle(v, eba_endian_little);
	failures += test_toggle(v, eba_big_endian);
	failures += test_toggle_all(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
