/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-shift-fill.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "eba-test-private-utils.h"
#include <limits.h>		/* CHAR_BIT */

int eba_test_shift_fill_endian(int verbose, unsigned char fill,
			       enum eba_endian endian)
{
	int failures = 0;
	size_t i = 0;
	unsigned char bytes[10];
	unsigned char start[10];
	unsigned char middle[10];
	unsigned char end[10];
	struct eba eba;
	unsigned int shift_amount = 0;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_shift_fill_endian", endian);

	for (i = 0; i < 10; ++i) {
		bytes[i] = 0;
		start[i] = 0;
		middle[i] = 0;
		end[i] = 0;
	}

	eba.bits = bytes;
	eba.size_bytes = 10;
	eba.endian = endian;

	start[0] = (1U << 0);
	start[1] = (1U << 2);
	start[3] = (1U << 0);
	start[8] = ((1U << 0) | (1U << 3) | (1U << 5));
	start[9] = ((1U << 0) | (1U << 4) | (1U << 7));

	shift_amount = 19;
	if (endian == eba_endian_little) {
		if (fill) {
			middle[0] |= 0xFF;
			middle[1] |= 0xFF;
			middle[2] |= 0x07;
		}

		middle[2] = ((middle[2]) | (1U << 3));
		middle[3] = (1U << 5);
		middle[5] = (1U << 3);

		end[0] = (1U << 0);
		end[1] = (1U << 2);
		end[3] = (1U << 0);

		if (fill) {
			end[9] = 0xFF;
			end[8] = 0xFF;
			end[7] = (end[7] | (1U << 5) | (1U << 6) | (1U << 7));
		}
	} else {
		if (fill) {
			middle[9] |= 0xFF;
			middle[8] |= 0xFF;
			middle[7] |= 0x07;
		}
		middle[1] = (1U << 3);
		middle[5] = (1U << 0);
		middle[6] = ((1U << 3) | (1U << 6) | (1U << 2));
		middle[7] = ((middle[7]) | (1U << 3) | (1U << 7));

		end[3] = (1U << 0);
		end[8] = ((1U << 0) | (1U << 3) | (1U << 5));
		end[9] = ((1U << 0) | (1U << 4) | (1U << 7));

		if (fill) {
			end[0] = 0xFF;
			end[1] = 0xFF;
			end[2] = (end[7] | (1U << 5) | (1U << 6) | (1U << 7));
		}
	}

	for (i = 0; i < 10; ++i) {
		bytes[i] = start[i];
	}

	eba_shift_left_fill(&eba, shift_amount, fill);

	failures += check_byte_array_m(bytes, 10, middle, 10, "first");

	eba_shift_right_fill(&eba, shift_amount, fill);

	failures += check_byte_array_m(bytes, 10, end, 10, "second");

	if (failures) {
		Test_log_error(failures, "test_shift_fill");
	}

	return failures;
}

int eba_test_shift_right(int verbose, enum eba_endian endian, unsigned char *in,
			 size_t len, unsigned shift_amount,
			 const unsigned char *expected)
{
	int failures = 0;
	struct eba eba;

	eba.bits = in;
	eba.size_bytes = len;
	eba.endian = endian;

	VERBOSE_ANNOUNCE_S_Z(verbose, "eba_test_shift_right", shift_amount);

	eba_shift_right(&eba, shift_amount);

	failures += check_byte_array(in, len, expected, len);

	if (failures) {
		Test_log_error(failures, "test_shift_right");
	}

	return failures;
}

int eba_test_shift_fill(int v)
{
	int failures = 0;
	unsigned char fill = 0;
	unsigned int len = 20;
	unsigned int shift_amount = 0;
	unsigned char in[20], out[20];

	for (fill = 0; fill < 2; ++fill) {
		failures += eba_test_shift_fill_endian(v, fill, eba_big_endian);
		failures +=
		    eba_test_shift_fill_endian(v, fill, eba_endian_little);
	}

	eba_memset(in, 0x00, len);
	eba_memset(out, 0x00, len);
	in[18] = 0x03;
	shift_amount = 4;
	out[19] = 0x30;
	failures +=
	    eba_test_shift_right(v, eba_big_endian, in, len, shift_amount, out);

	eba_memset(in, 0x00, len);
	eba_memset(out, 0x00, len);
	in[18] = 0x03;
	shift_amount = 4;
	out[17] = 0x30;
	failures +=
	    eba_test_shift_right(v, eba_endian_little, in, len, shift_amount,
				 out);

	eba_memset(in, 0x00, len);
	eba_memset(out, 0x00, len);
	in[12] = 0x13;
	in[17] = 0x03;
	in[18] = 0x05;
	shift_amount = (len * CHAR_BIT) + 7;
	failures +=
	    eba_test_shift_right(v, eba_endian_little, in, len, shift_amount,
				 out);

	if (failures) {
		Test_log_error(failures, __FILE__);
	}

	return failures;
}

EBA_TEST(eba_test_shift_fill)
