/*
test-toggle.c
Copyright (C) 2016 Eric Herman <eric@freesa.org>

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

int test_toggle(int verbose)
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

	expected[0] = 4;
	expected[1] = 6;
	expected[6] = 255;
	expected[8] = 251;

	for (i = 0; i < 10; ++i) {
		bytes[i] = ~(expected[i]);
	}

	eba_toggle(&eba, 2);

	/* test indexing into second byte, twice */
	eba_toggle(&eba, 9);
	eba_toggle(&eba, 10);

	/* test toggling each bit of a byte */
	eba_toggle(&eba, 48);
	eba_toggle(&eba, 49);
	eba_toggle(&eba, 50);
	eba_toggle(&eba, 51);
	eba_toggle(&eba, 52);
	eba_toggle(&eba, 53);
	eba_toggle(&eba, 54);
	eba_toggle(&eba, 55);

	/* test toggling each bit of a byte, then set one bit back to zero */
	eba_toggle(&eba, 64);
	eba_toggle(&eba, 65);
	eba_toggle(&eba, 66);
	eba_toggle(&eba, 67);
	eba_toggle(&eba, 68);
	eba_toggle(&eba, 69);
	eba_toggle(&eba, 70);
	eba_toggle(&eba, 71);
	eba_toggle(&eba, 66);

	failures = check_byte_array(bytes, 10, expected, 10);

	if (failures) {
		Test_log_error1("%d failures in test_dec\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_toggle(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
