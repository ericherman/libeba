/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

int test_to_string_be(void)
{
	int failures = 0;
	struct eba_s *eba;
	char buf[40];

	eba = eba_new_endian(10, eba_big_endian);
	if (!eba) {
		return 1;
	}

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);
	eba_free(eba);

	failures += check_str(buf, "00000001 00101110");

	return failures;
}

int test_to_string_el(void)
{
	int failures = 0;
	struct eba_s *eba;
	char buf[40];

	eba = eba_new_endian(10, eba_endian_little);
	if (!eba) {
		return 1;
	}

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eba_to_string(eba, buf, 40);
	eba_free(eba);

	failures += check_str(buf, "01110100 10000000");

	return failures;
}

int test_to_string_error_1(void)
{
	int failures = 0;
	struct eba_s *eba;
	char buf[40];

	eba = eba_new_endian(10, eba_big_endian);
	if (!eba) {
		return 1;
	}

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 11);
	eba_free(eba);

	failures += check_str(buf, "00000001 !");

	return failures;
}

int test_to_string_error_2(void)
{
	int failures = 0;
	struct eba_s *eba;
	char buf[40];

	eba = eba_new_endian(10, eba_big_endian);
	if (!eba) {
		return 1;
	}

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 2);
	eba_free(eba);

	failures += check_str(buf, "!");

	return failures;
}

int main(int argc, char **argv)
{
	int verbose, failures;

	verbose = (argc > 1) ? atoi(argv[1]) : 0;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	failures += test_to_string_be();
#if (!(EBA_SKIP_ENDIAN))
	failures += test_to_string_el();
#endif

	failures += test_to_string_error_1();
	failures += test_to_string_error_2();

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
