/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"

#define Num_bits 100

unsigned eba_test_new_inner(struct eba *eba)
{
	unsigned failures = 0;
	size_t i;
	unsigned char expect;

	if (!eba) {
		eembed_err_log->append_s(eembed_err_log, "no eba?");
		eembed_err_log->append_eol(eembed_err_log);
		return 1;
	}
	if (!eba->size_bytes) {
		eembed_err_log->append_s(eembed_err_log, "no eba->size_bytes?");
		eembed_err_log->append_eol(eembed_err_log);
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

unsigned eba_test_new(int verbose)
{
	unsigned failures = 0;
	struct eba *eba = NULL;
	void *can_alloc = NULL;

	VERBOSE_ANNOUNCE_S(verbose, "eba_test_new");
	failures = 0;

	can_alloc = eembed_malloc(sizeof(int));
	if (!can_alloc) {
		if (EEMBED_HOSTED) {
			eembed_err_log->append_s(eembed_err_log,
						 "failed to allocate an int?");
			eembed_err_log->append_eol(eembed_err_log);
		}
		VERBOSE_ANNOUNCE_DONE(verbose, EEMBED_HOSTED);
		return EEMBED_HOSTED;
	}
	eembed_free(can_alloc);

	eba = eba_new(Num_bits);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	eba = eba_new_endian(Num_bits, eba_big_endian);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	eba = eba_new_endian(Num_bits, eba_endian_little);
	failures += eba_test_new_inner(eba);
	eba_free(eba);

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

ECHECK_TEST_MAIN_V(eba_test_new)
