/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* eba-test-private-utils.h */
/* Copyright (C) 2016, 2017, 2019 Eric Herman <eric@freesa.org> */

#ifndef EBA_TEST_PRIVATE_UTILS_H
#define EBA_TEST_PRIVATE_UTILS_H

#include "eba.h"
#include "echeck.h"

#define VERBOSE_ANNOUNCE_S(verbose, msg) \
	do { \
		if (verbose) { \
			eembed_err_log->append_s(eembed_err_log, "running "); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_s(eembed_err_log, msg); \
			eembed_err_log->append_s(eembed_err_log, " ... "); \
		} \
	} while (0)

#define VERBOSE_ANNOUNCE_S_Z(verbose, msg, z) \
	do { \
		if (verbose) { \
			eembed_err_log->append_s(eembed_err_log, "running "); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_s(eembed_err_log, msg); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_ul(eembed_err_log, z); \
			eembed_err_log->append_s(eembed_err_log, " ... "); \
		} \
	} while (0)

#define VERBOSE_ANNOUNCE_S_Z_Z_Z(verbose, msg, x, y, z) \
	do { \
		if (verbose) { \
			eembed_err_log->append_s(eembed_err_log, "running "); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_s(eembed_err_log, msg); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_ul(eembed_err_log, x); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_ul(eembed_err_log, y); \
			eembed_err_log->append_s(eembed_err_log, " "); \
			eembed_err_log->append_ul(eembed_err_log, z); \
			eembed_err_log->append_s(eembed_err_log, " ... "); \
		} \
	} while (0)

#define VERBOSE_ANNOUNCE_DONE(verbose, failures) \
	do { \
		if (verbose) { \
			const char *cres = failures ? "FAIL!" : "ok"; \
			eembed_err_log->append_s(eembed_err_log, cres); \
			eembed_err_log->append_eol(eembed_err_log); \
		} \
	} while (0)

#endif /* EBA_TEST_PRIVATE_UTILS_H */
