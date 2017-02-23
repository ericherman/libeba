/*
eba-test-private-utils.h
Copyright (C) 2016 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/
#ifndef EBA_TEST_PRIVATE_UTILS_H
#define EBA_TEST_PRIVATE_UTILS_H

#include "../src/eba.h"
#include "echeck.h"
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
