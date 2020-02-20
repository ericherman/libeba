/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba-test-private-utils.h */
/* Copyright (C) 2016, 2017, 2019 Eric Herman <eric@freesa.org> */

#ifndef EBA_TEST_PRIVATE_UTILS_H
#define EBA_TEST_PRIVATE_UTILS_H

#include "../src/eba.h"
#include "../submodules/libecheck/src/echeck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_FUNC \
	((ECHECK_FUNC == NULL) ? "" : ECHECK_FUNC)

#define STDERR_FILE_LINE_FUNC \
	fprintf(stderr, "%s:%d%s%s%s: ", __FILE__, __LINE__, \
	(ECHECK_FUNC == NULL) ? "" : ":", \
	TEST_FUNC, \
	(ECHECK_FUNC == NULL) ? "" : "()")

#define Test_log_error(format) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format)

#define Test_log_error1(format, arg) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg)

#define Test_log_error2(format, arg1, arg2) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2)

#define Test_log_error3(format, arg1, arg2, arg3) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3)

#define Test_log_error4(format, arg1, arg2, arg3, arg4) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3, arg4)

#define VERBOSE_ANNOUNCE(verbose) \
	if (verbose) { fprintf(stderr, "starting %s\n", TEST_FUNC); }

#define cap_failures(failures) \
	((failures > 127) ? 127 : ((failures < -128) ? -128 : failures))

#endif /* EBA_TEST_PRIVATE_UTILS_H */
