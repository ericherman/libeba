/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-shift-right.c */
/* Copyright (C) 2018, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"
#include <limits.h>
#include <stdint.h>

int test_shift_right(int verbose, uint16_t u16, uint8_t shift_val,
		     enum eba_endian endian)
{
	int failures;
	unsigned char bytes[2];
	unsigned char expect_bytes[2];
	struct eba_s eba;
	struct eba_s expect;
	uint16_t expect_val;
	char msg[255];
	char eba_buf[40];
	char expect_buf[40];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;
	msg[0] = '\0';
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

	sprintf(msg, "%u >> %u == %u (0b%s)", (unsigned)u16,
		(unsigned)shift_val, (unsigned)expect_val, expect_buf);
	failures += check_str_m(eba_buf, expect_buf, msg);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	uint16_t u16;
	uint8_t shift_val;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;
	for (u16 = UINT16_MAX; !failures && u16; --u16) {
		for (shift_val = 16; shift_val; --shift_val) {
#if Eba_need_endian
			failures +=
			    test_shift_right(v, u16, shift_val,
					     eba_endian_little);
#endif
			failures +=
			    test_shift_right(v, u16, shift_val, eba_big_endian);
		}
	}

	return cap_failures(failures);
}
