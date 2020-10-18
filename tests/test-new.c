/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

#define Num_bits 100

int eba_test_new_inner(struct eba *eba)
{
	int failures = 0;
	size_t i;
	unsigned char expect;

	if (!eba) {
		eba_debug_print_s("no eba?");
		eba_debug_print_eol();
		return 1;
	}
	if (!eba->size_bytes) {
		eba_debug_print_s("no eba->size_bytes?");
		eba_debug_print_eol();
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
		failures += check_int(eba_get(eba, i), expect);
	}

	return failures;
}

int eba_test_new(int verbose)
{
	int failures = 0;
	struct eba *eba = NULL;
	void *can_alloc = NULL;

#if !EBA_HOSTED
	if (!eba_alloc) {
		return 0;
	}
#endif
	can_alloc = eba_alloc(eba_alloc_context, sizeof(int));
	if (!can_alloc) {
		return EBA_HOSTED;
	}
	eba_mfree(eba_alloc_context, can_alloc);

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_new");
	failures = 0;

	eba = eba_new(Num_bits);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	eba = eba_new_endian(Num_bits, eba_big_endian);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	eba = eba_new_endian(Num_bits, eba_endian_little);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return failures;
}

EBA_TEST(eba_test_new)
