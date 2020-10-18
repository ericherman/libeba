/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-shift-left.c */
/* Copyright (C) 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"
#include <limits.h>
#include <stdint.h>

int eba_test_shift_left_inner(int verbose, uint16_t u16, uint8_t shift_val,
			      enum eba_endian endian)
{
	int failures;
	unsigned char bytes[2];
	unsigned char expect_bytes[2];
	struct eba eba;
	struct eba expect;
	uint16_t expect_val;
	char eba_buf[40];
	char expect_buf[40];

	VERBOSE_ANNOUNCE_S_Z_Z_Z(verbose, "eba_test_shift_left_inner", endian,
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

	expect_val = (u16 << shift_val);
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

	eba_shift_left(&eba, shift_val);
	eba_to_string(&eba, eba_buf, 40);

	failures += check_str(eba_buf, expect_buf);

	if (failures) {
		Test_log_error(failures, "test_shift_left");
	}
	return failures;
}

int eba_test_shift_left_bug(int verbose, enum eba_endian endian)
{
	int failures;
	unsigned char bytes[20];
	unsigned char expect_bytes[20];
	struct eba eba;
	struct eba expect;
	char eba_buf[255];
	char expect_buf[255];

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_shift_left_bug", endian);
	failures = 0;
	eba_buf[0] = '\0';
	expect_buf[0] = '\0';

	eba.bits = bytes;
	eba.size_bytes = 20;
	eba_memset(eba.bits, 0x00, eba.size_bytes);
	eba.endian = endian;

	expect.bits = expect_bytes;
	expect.size_bytes = 20;
	eba_memset(expect.bits, 0x00, expect.size_bytes);
	expect.endian = endian;

	if (endian == eba_endian_little) {
		bytes[0] = 0x03;
		expect_bytes[1] = 0x03;
		eba_to_string(&expect, expect_buf, 255);
	} else {
		bytes[eba.size_bytes - 1] = 0x03;
		expect_bytes[expect.size_bytes - 2] = 0x03;
		eba_to_string(&expect, expect_buf, 255);
	}

	eba_shift_left(&eba, CHAR_BIT);
	eba_to_string(&eba, eba_buf, 255);

	failures += check_str(eba_buf, expect_buf);

	if (failures) {
		Test_log_error(failures, "test_shift_left_bug");
	}
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

int eba_test_shift_left(int v)
{
	int failures = 0;
	uint16_t u16 = 0;
	uint8_t shift_val = 0;

	for (u16 = UINT16_MAX; !failures && u16; u16 = next_u16_test(u16)) {
		for (shift_val = 16; shift_val; --shift_val) {
			failures +=
			    eba_test_shift_left_inner(v, u16, shift_val,
						      eba_endian_little);
			failures +=
			    eba_test_shift_left_inner(v, u16, shift_val,
						      eba_big_endian);
		}
	}

	failures += eba_test_shift_left_bug(v, eba_big_endian);
	failures += eba_test_shift_left_bug(v, eba_endian_little);

	if (failures) {
		Test_log_error(failures, __FILE__);
	}
	return failures;
}

EBA_TEST(eba_test_shift_left)
