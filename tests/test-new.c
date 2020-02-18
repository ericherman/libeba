/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

#define Num_bits 100

int test_new_inner(struct eba_s *eba)
{
	int failures = 0;
	size_t i;
	unsigned char expect;
	char buf[40];

	if (!eba) {
		fprintf(stderr, "no eba?\n");
		return 1;
	}
	if (!eba->size_bytes) {
		fprintf(stderr, "no eba->size_bytes?\n");
		return 1;
	}

	for (i = 0; i < Num_bits; ++i) {
		if (eba_get(eba, i)) {
			++failures;
		}
	}
	for (i = 0; i < Num_bits; ++i) {
		if (i > 50) {
			eba_set(eba, i, 0);
		}
		if ((i % 2) == 0) {
			eba_set(eba, i, 1);
		}
		if ((i % 4) == 0) {
			eba_set(eba, i, 0);
		}
		if ((i % 8) == 0) {
			eba_set(eba, i, 1);
		}
		if (i > 75) {
			eba_set(eba, i, eba_get(eba, i));
		}
	}
	for (i = 0; i < Num_bits; ++i) {
		if (((i % 2) == 0) && (((i % 4) != 0) || ((i % 8) == 0))) {
			expect = 1;
		} else {
			expect = 0;
		}
		sprintf(buf, "%lu", (unsigned long)i);
		failures += check_int_m(eba_get(eba, i), expect, buf);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int verbose, failures;
	struct eba_s *eba;

	verbose = (argc > 1) ? atoi(argv[1]) : 0;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	eba = eba_new(Num_bits);
	failures += test_new_inner(eba);
	eba_free(eba);

	eba = eba_new_endian(Num_bits, eba_big_endian);
	failures += test_new_inner(eba);
	eba_free(eba);

#if !((!(EBA_SKIP_ENDIAN)))
	eba = eba_new_endian(Num_bits, eba_endian_little);
	failures += test_new_inner(eba);
	eba_free(eba);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
