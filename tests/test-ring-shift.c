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

#if EBA_SKIP_ENDIAN
int test_ring_shift(int verbose)
#else
int test_ring_shift(int verbose, enum eba_endian endian)
#endif
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char start[10];
	unsigned char middle[10];
	struct eba_s eba;
	unsigned int shift_amount;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		start[i] = 0;
		middle[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
#if Eba_need_endian
	eba.endian = endian;
#endif

	start[1] = (1U << 2);
	start[3] = (1U << 0);
	start[8] = ((1U << 0) | (1U << 3) | (1U << 5));
	start[9] = ((1U << 0) | (1U << 4) | (1U << 7));

	shift_amount = 19;
#if Eba_need_endian
	if (endian == eba_endian_little) {
#endif
		middle[0] = ((1U << 3) | (1U << 6));
		middle[1] = ((1U << 0) | (1U << 3) | (1U << 7));
		middle[2] = (1U << 2);
		middle[3] = (1U << 5);
		middle[5] = (1U << 3);
#if Eba_need_endian
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
#endif

	for (i = 0; i < 10; ++i) {
		bytes[i] = start[i];
	}

	eba_ring_shift_left(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 10, middle, 10, "first");

	eba_ring_shift_right(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 10, start, 10, "second");

	if (failures) {
		Test_log_error1("%d failures in test_set\n", failures);
	}

	return failures;
}

int test_simple_ring_shift(int verbose)
{
	int failures;
	size_t i;
	unsigned char bytes[2];
	unsigned char start[2];
	unsigned char middle[2];
	struct eba_s eba;
	unsigned long shift_amount;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < 2; ++i) {
		bytes[i] = 0;
		start[i] = 0;
		middle[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 2;
#if Eba_need_endian
	eba.endian = eba_endian_little;
#endif

	start[0] = 5;
	start[1] = 8;

	for (i = 0; i < 2; ++i) {
		bytes[i] = start[i];
	}

	shift_amount = 2;
	eba_ring_shift_left(&eba, shift_amount);

	middle[0] = (5U << 2);
	middle[1] = (8U << 2);

	failures += check_byte_array_m(bytes, 2, middle, 2, "first");

	eba_ring_shift_right(&eba, shift_amount);

	failures += check_byte_array_m(bytes, 2, start, 2, "second");

	if (failures) {
		Test_log_error1("%d failures in test_shift\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_simple_ring_shift(v);

#if EBA_SKIP_ENDIAN
	failures += test_ring_shift(v);
#else
	failures += test_ring_shift(v, eba_endian_little);
	failures += test_ring_shift(v, eba_big_endian);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
