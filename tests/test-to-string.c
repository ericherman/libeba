/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int test_to_string_be(void)
{
	int failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);

	failures += check_str(buf, "00000001 00101110");

	if (failures) {
		Test_log_error(failures, "test_to_string_be");
	}
	return failures;
}

int test_to_string_el(void)
{
	int failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_endian_little;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);

	failures += check_str(buf, "01110100 10000000");

	if (failures) {
		Test_log_error(failures, "test_to_string_el");
	}
	return failures;
}

int test_to_string_error_1(void)
{
	int failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 11);

	failures += check_str(buf, "00000001 !");

	if (failures) {
		Test_log_error(failures, "test_to_string_error_1");
	}
	return failures;
}

int test_to_string_error_2(void)
{
	int failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	eba_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 2);

	failures += check_str(buf, "!");

	if (failures) {
		Test_log_error(failures, "test_to_string_error_2");
	}
	return failures;
}

int test_to_string_invalid(void)
{
	int failures = 0;
	struct eba eba;
	unsigned char bytes[2];
	char buf[40];
	char *rv;

	eba_memset(bytes, 0x00, 2);
	eba.bits = bytes;
	eba.size_bytes = 2;
	eba.endian = eba_big_endian;

	eba_memset(buf, 'Z', 39);
	buf[39] = '\0';

	rv = eba_to_string(&eba, NULL, 40);
	failures += check_ptr(rv, NULL);

	rv = eba_to_string(&eba, buf, 0);
	failures += check_ptr(rv, NULL);

	rv = eba_to_string(&eba, buf, 1);
	failures += check_ptr(rv, NULL);

	rv = eba_to_string(NULL, buf, 40);
	failures += check_ptr(rv, buf);
	failures += check_str(buf, "");

	eba.bits = NULL;
	rv = eba_to_string(NULL, buf, 40);
	failures += check_ptr(rv, buf);
	failures += check_str(buf, "");

	if (failures) {
		Test_log_error(failures, "test_to_string_invalid");
	}
	return failures;
}

int main(int argc, char **argv)
{
	int verbose, failures;

	verbose = (argc > 1) ? atoi(argv[1]) : 0;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	failures += test_to_string_be();
	failures += test_to_string_el();

	failures += test_to_string_error_1();
	failures += test_to_string_error_2();
	failures += test_to_string_invalid();

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return cap_failures(failures);
}
