/*
test-shift-left.c
Copyleft (C) 2018 Eric Herman <eric@freesa.org>

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
#include <limits.h>
#include <stdint.h>

#if EBA_SKIP_ENDIAN
int test_shift_left(int verbose, uint16_t u16, uint8_t shift_val)
#else
int test_shift_left(int verbose, uint16_t u16, uint8_t shift_val,
		    enum eba_endian endian)
#endif
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
#if Eba_need_endian
	eba.endian = endian;
#endif
	expect.bits = expect_bytes;
	expect.size_bytes = 2;
#if Eba_need_endian
	expect.endian = endian;
#endif

	expect_val = (u16 << shift_val);
#if Eba_need_endian
	if (endian == eba_endian_little) {
		bytes[0] = u16 & 0xFF;
		bytes[1] = (u16 >> 8) & 0xFF;

		expect_bytes[0] = expect_val & 0xFF;
		expect_bytes[1] = (expect_val >> 8) & 0xFF;
		eba_to_string(&expect, expect_buf, 40);
	} else {
#endif
		bytes[1] = u16 & 0xFF;
		bytes[0] = (u16 >> 8) & 0xFF;

		expect_bytes[1] = expect_val & 0xFF;
		expect_bytes[0] = (expect_val >> 8) & 0xFF;
		eba_to_string(&expect, expect_buf, 40);
#if Eba_need_endian
	}
#endif

	eba_shift_left(&eba, shift_val);
	eba_to_string(&eba, eba_buf, 40);

	sprintf(msg, "%u << %u == %u (0b%s)", (unsigned)u16,
		(unsigned)shift_val, (unsigned)expect_val, expect_buf);
	failures += check_str_m(eba_buf, expect_buf, msg);

	return failures;
}

#if EBA_SKIP_ENDIAN
int test_shift_left_bug(int verbose)
#else
int test_shift_left_bug(int verbose, enum eba_endian endian)
#endif
{
	int failures;
	unsigned char bytes[20];
	unsigned char expect_bytes[20];
	struct eba_s eba;
	struct eba_s expect;
	char msg[1024];
	char eba_buf[255];
	char expect_buf[255];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;
	msg[0] = '\0';
	eba_buf[0] = '\0';
	expect_buf[0] = '\0';

	eba.bits = bytes;
	eba.size_bytes = 20;
	memset(eba.bits, 0x00, eba.size_bytes);
#if Eba_need_endian
	eba.endian = endian;
#endif
	expect.bits = expect_bytes;
	expect.size_bytes = 20;
	memset(expect.bits, 0x00, expect.size_bytes);
#if Eba_need_endian
	expect.endian = endian;
#endif

#if Eba_need_endian
	if (endian == eba_endian_little) {
		bytes[0] = 0x03;
		expect_bytes[1] = 0x03;
		eba_to_string(&expect, expect_buf, 255);
	} else {
#endif
		bytes[eba.size_bytes - 1] = 0x03;
		expect_bytes[expect.size_bytes - 2] = 0x03;
		eba_to_string(&expect, expect_buf, 255);
#if Eba_need_endian
	}
#endif

	eba_shift_left(&eba, CHAR_BIT);
	eba_to_string(&eba, eba_buf, 255);

	sprintf(msg, "%u << %u == %u (0b%s)", (unsigned)0x03,
		(unsigned)CHAR_BIT, (unsigned)0x0300, expect_buf);
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
#if EBA_SKIP_ENDIAN
			failures += test_shift_left(v, u16, shift_val);
#else
			failures +=
			    test_shift_left(v, u16, shift_val,
					    eba_endian_little);
			failures +=
			    test_shift_left(v, u16, shift_val, eba_big_endian);
#endif
		}
	}

#if EBA_SKIP_ENDIAN
	failures += test_shift_left_bug(v);
#else
	failures += test_shift_left_bug(v, eba_big_endian);
	failures += test_shift_left_bug(v, eba_endian_little);
#endif
	return cap_failures(failures);
}
