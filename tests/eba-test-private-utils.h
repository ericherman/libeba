/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba-test-private-utils.h */
/* Copyright (C) 2016, 2017, 2019 Eric Herman <eric@freesa.org> */

#ifndef EBA_TEST_PRIVATE_UTILS_H
#define EBA_TEST_PRIVATE_UTILS_H

#include "eba.h"
#include <stdint.h>

#ifdef ARDUINO
#ifndef EBA_HOSTED
#define EBA_HOSTED 0
#define EBA_TEST_SKIP_MAIN 1
#endif
#ifndef EBA_DEBUG
#define EBA_DEBUG 0
#endif
#endif /* ARDUINO */

#ifndef EBA_HOSTED
#define EBA_HOSTED __STDC_HOSTED__
#endif

#include "echeck.h"

#define Test_log_error(num_errors, in_what) \
	do { \
		eba_debug_print_s(__FILE__); \
		eba_debug_print_s(":"); \
		eba_debug_print_z(__LINE__); \
		eba_debug_print_s(" "); \
		eba_debug_print_z(num_errors); \
		eba_debug_print_s(" failures in "); \
		eba_debug_print_s(in_what); \
		eba_debug_print_eol(); \
	} while (0)

#define VERBOSE_ANNOUNCE_S(verbose, msg) \
	do { \
		if (verbose) { \
			eba_debug_print_s("starting "); \
			eba_debug_print_s(" "); \
			eba_debug_print_s(msg); \
			eba_debug_print_eol(); \
		} \
	} while (0)

#define VERBOSE_ANNOUNCE_S_Z(verbose, msg, z) \
	do { \
		if (verbose) { \
			eba_debug_print_s("starting "); \
			eba_debug_print_s(" "); \
			eba_debug_print_s(msg); \
			eba_debug_print_s(" "); \
			eba_debug_print_z((size_t)z); \
			eba_debug_print_eol(); \
		} \
	} while (0)

#define VERBOSE_ANNOUNCE_S_Z_Z_Z(verbose, msg, x, y, z) \
	do { \
		if (verbose) { \
			eba_debug_print_s("starting "); \
			eba_debug_print_s(" "); \
			eba_debug_print_s(msg); \
			eba_debug_print_s(" "); \
			eba_debug_print_z((size_t)x); \
			eba_debug_print_s(" "); \
			eba_debug_print_z((size_t)y); \
			eba_debug_print_s(" "); \
			eba_debug_print_z((size_t)z); \
			eba_debug_print_eol(); \
		} \
	} while (0)

#define cap_failures(failures) \
	((failures > 127) ? 127 : ((failures < -128) ? -128 : failures))

#ifndef EBA_TEST_SKIP_MAIN
#define EBA_TEST_SKIP_MAIN 0
#endif

#if (EBA_TEST_SKIP_MAIN)
#define EBA_TEST(func)		/* EMPTY */
#else
#define EBA_TEST(func) \
int main(int argc, char **argv) \
{ \
	int v, failures; \
	v = (argc > 1) ? atoi(argv[1]) : 0; \
	failures = func(v); \
	return cap_failures(failures); \
}
#endif

#endif /* EBA_TEST_PRIVATE_UTILS_H */
