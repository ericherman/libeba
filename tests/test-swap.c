/*
test-swap.c
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

int test_swap_endianness(int verbose, enum eba_endian endian)
{
	int failures;
	size_t i, j;
	unsigned char bytes[10];
	unsigned char expected[10];
	struct eba_s eba;
	unsigned int index1, index2;

	VERBOSE_ANNOUNCE(verbose);
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
		Test_log_error1("%d failures in test_set\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

#if Eba_need_endian
	failures += test_swap_endianness(v, eba_endian_little);
#endif
	failures += test_swap_endianness(v, eba_big_endian);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
