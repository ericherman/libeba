/*
test-shift-fill.c
Copyright (C) 2017 Eric Herman <eric@freesa.org>

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
#include <limits.h>		/* CHAR_BIT */

int test_shift_fill(int verbose, unsigned char fill, enum eba_endian endian)
{
	int failures;
	size_t i;
	unsigned char bytes[10];
	unsigned char start[10];
	unsigned char middle[10];
	unsigned char end[10];
	struct eba_s eba;
	unsigned int shift_amount;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

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
		Test_log_error1("%d failures in test_shift_fill\n", failures);
	}

	return failures;
}

int test_shift_right(int verbose, enum eba_endian endian, unsigned char *in,
		     size_t len, unsigned shift_amount,
		     const unsigned char *expected)
{
	int failures;

	struct eba_s eba;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	eba.bits = in;
	eba.size_bytes = len;
	eba.endian = endian;

	eba_shift_right(&eba, shift_amount);

	failures += check_byte_array(in, len, expected, len);

	if (failures) {
		Test_log_error1("%d failures in test_shift_fill\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	unsigned char fill;
	unsigned int len;
	unsigned int shift_amount;
	unsigned char in[20], out[20];

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;
	len = 20;

	for (fill = 0; fill < 2; ++fill) {
		failures += test_shift_fill(v, fill, eba_big_endian);
#if Eba_need_endian
		failures += test_shift_fill(v, fill, eba_endian_little);
#endif
	}

	memset(in, 0x00, len);
	memset(out, 0x00, len);
	in[18] = 0x03;
	shift_amount = 4;
	out[19] = 0x30;
	failures +=
	    test_shift_right(v, eba_big_endian, in, len, shift_amount, out);

#if Eba_need_endian
	memset(in, 0x00, len);
	memset(out, 0x00, len);
	in[18] = 0x03;
	shift_amount = 4;
	out[17] = 0x30;
	failures +=
	    test_shift_right(v, eba_endian_little, in, len, shift_amount, out);
#endif

#if Eba_need_endian
	memset(in, 0x00, len);
	memset(out, 0x00, len);
	in[12] = 0x13;
	in[17] = 0x03;
	in[18] = 0x05;
	shift_amount = (len * CHAR_BIT) + 7;
	failures +=
	    test_shift_right(v, eba_endian_little, in, len, shift_amount, out);
#endif

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
