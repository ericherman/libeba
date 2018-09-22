/*
test-set-all.c
Copyright (C) 2018 Eric Herman <eric@freesa.org>

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
int test_set_all(int verbose)
#else
int test_set_all(int verbose, enum eba_endian endian)
#endif
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
#if Eba_need_endian
	eba.endian = endian;
#endif

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

#if EBA_SKIP_ENDIAN
	failures += test_set_all(v);
#else
	failures += test_set_all(v, eba_endian_little);
	failures += test_set_all(v, eba_big_endian);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
