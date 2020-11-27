/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-shift-right.c */
/* Copyright (C) 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"
#include <limits.h>
#include <stdint.h>

unsigned eba_test_shift_right_endian(int verbose, uint16_t u16,
				     uint8_t shift_val, enum eba_endian endian)
{
	unsigned failures;
	unsigned char bytes[2];
	unsigned char expect_bytes[2];
	struct eba eba;
	struct eba expect;
	uint16_t expect_val;
	char eba_buf[40];
	char expect_buf[40];

	VERBOSE_ANNOUNCE_S_Z_Z_Z(verbose, "eba_test_shift_right_endian", endian,
				 u16, shift_val);
	failures = 0;
	eba_buf[0] = '\0';
	expect_buf[0] = '\0';

	eba.bits = bytes;
	eba.size_bytes = 2;
	eba.endian = endian;

	expect.bits = expect_bytes;
	expect.size_bytes = 2;
	expect.endian = endian;

	expect_val = (u16 >> shift_val);
	if (endian == eba_endian_little) {
		bytes[0] = u16 & 0xFF;
		bytes[1] = (u16 >> 8) & 0xFF;

		expect_bytes[0] = expect_val & 0xFF;
		expect_bytes[1] = (expect_val >> 8) & 0xFF;
		eba_to_string(&expect, expect_buf, 40);
	} else {
		bytes[1] = u16 & 0xFF;
		bytes[0] = (u16 >> 8) & 0xFF;

		expect_bytes[1] = expect_val & 0xFF;
		expect_bytes[0] = (expect_val >> 8) & 0xFF;
		eba_to_string(&expect, expect_buf, 40);
	}

	eba_shift_right(&eba, shift_val);
	eba_to_string(&eba, eba_buf, 40);

	failures += check_str(eba_buf, expect_buf);

	VERBOSE_ANNOUNCE_DONE(verbose, failures);

	return failures;
}

static uint16_t next_u16_test(uint16_t in)
{
	if (in >= (UINT16_MAX - (CHAR_BIT + 1)) || in <= (CHAR_BIT + 1)) {
		return in - 1;
	}
	if (in > 8192) {
		return in - 7993;
	}
	if (in > 255) {
		return in - 251;
	}
	if (in > 32) {
		return in - 17;
	}
	return in - 1;
}

unsigned eba_test_shift_right(int v)
{
	unsigned failures = 0;
	uint16_t u16 = 0;
	uint8_t shift_val = 0;

	for (u16 = UINT16_MAX; !failures && u16; u16 = next_u16_test(u16)) {
		for (shift_val = 16; shift_val; --shift_val) {
			failures +=
			    eba_test_shift_right_endian(v, u16, shift_val,
							eba_endian_little);
			failures +=
			    eba_test_shift_right_endian(v, u16, shift_val,
							eba_big_endian);
		}
	}

	return failures;
}

ECHECK_TEST_MAIN_V(eba_test_shift_right)
