/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

unsigned eba_test_to_string_be(int verbose)
{
	unsigned failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_to_string_be");

	eembed_memset(bytes, 0x00, 2);
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

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_to_string_el(int verbose)
{
	unsigned failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_to_string_el");

	eembed_memset(bytes, 0x00, 2);
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

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_to_string_error_1(int verbose)
{
	unsigned failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_to_string_error_1");

	eembed_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eembed_memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 11);

	failures += check_str(buf, "00000001 !");

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_to_string_error_2(int verbose)
{
	unsigned failures = 0;
	unsigned char bytes[2];
	struct eba eba_s;
	struct eba *eba = &eba_s;
	char buf[40];

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_to_string_error_2");

	eembed_memset(bytes, 0x00, 2);
	eba->bits = bytes;
	eba->size_bytes = 2;
	eba->endian = eba_big_endian;

	eba_set(eba, 1, 1);
	eba_set(eba, 2, 1);
	eba_set(eba, 3, 1);
	eba_set(eba, 5, 1);
	eba_set(eba, 8, 1);

	eembed_memset(buf, 0x00, 40);
	eba_to_string(eba, buf, 2);

	failures += check_str(buf, "!");

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

unsigned eba_test_to_string_invalid(int verbose)
{
	unsigned failures = 0;
	struct eba eba;
	unsigned char bytes[2];
	char buf[40];
	char *rv;

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_to_string_invalid");

	eembed_memset(bytes, 0x00, 2);
	eba.bits = bytes;
	eba.size_bytes = 2;
	eba.endian = eba_big_endian;

	eembed_memset(buf, 'Z', 39);
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

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

/* TODO Split in to five tests */
unsigned eba_test_to_string(int verbose)
{
	unsigned failures = 0;

	failures += eba_test_to_string_be(verbose);
	failures += eba_test_to_string_el(verbose);

	failures += eba_test_to_string_error_1(verbose);
	failures += eba_test_to_string_error_2(verbose);
	failures += eba_test_to_string_invalid(verbose);

	return failures;
}

ECHECK_TEST_MAIN_V(eba_test_to_string)
