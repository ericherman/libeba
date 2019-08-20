/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* echeck.h */
/* libecheck: "E(asy)Check" boiler-plate to make simple testing easier */
/* Copyright (C) 2016, 2017, 2018, 2019 Eric Herman <eric@freesa.org> */

#ifndef ECHECK_H
#define ECHECK_H

#ifdef __cplusplus
#define Echeck_begin_C_functions extern "C" {
#define Echeck_end_C_functions }
#else
#define Echeck_begin_C_functions
#define Echeck_end_C_functions
#endif

#include <stdlib.h>		/* for size_t */
#include <stdio.h>		/* for FILE */
#include <float.h>		/* for DBL_EPSILON */

#ifdef _GNU_SOURCE
#define ECHECK_FUNC __PRETTY_FUNCTION__
#else
#if (__STDC_VERSION__ >= 199901L)
#define ECHECK_FUNC __func__
#else
#define ECHECK_FUNC NULL
#endif /* _GNU_SOURCE */
#endif /* __STDC_VERSION__ */

Echeck_begin_C_functions
/*check char*/
int echeck_char_m(FILE *err, const char *func, const char *file, int line,
		  char actual, char expected, const char *msg);

#define fcheck_char_m(log, actual, expected, msg)\
	echeck_char_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_char_m(actual, expected, msg)\
	echeck_char_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_char(log, actual, expected)\
	echeck_char_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_char(actual, expected)\
	echeck_char_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check long */
int echeck_long_m(FILE *err, const char *func, const char *file, int line,
		  long actual, long expected, const char *msg);

#define fcheck_long_m(log, actual, expected, msg)\
	echeck_long_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_long_m(actual, expected, msg)\
	echeck_long_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_long(log, actual, expected)\
	echeck_long_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_long(actual, expected)\
	echeck_long_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check int */
#define echeck_int_m(err, func, file, line, actual, expected, msg)\
	echeck_long_m(err, func, file, line, actual, expected, msg)

#define fcheck_int_m(log, actual, expected, msg)\
	echeck_int_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_int_m(actual, expected, msg)\
	echeck_int_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_int(log, actual, expected)\
	echeck_int_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_int(actual, expected)\
	echeck_int_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check str */
int echeck_str_m(FILE *err, const char *func, const char *file, int line,
		 const char *actual, const char *expected, const char *msg);

#define fcheck_str_m(log, actual, expected, msg)\
	echeck_str_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_str_m(actual, expected, msg)\
	echeck_str_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_str(log, actual, expected)\
	echeck_str_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_str(actual, expected)\
	echeck_str_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check ptr */
int echeck_ptr_m(FILE *err, const char *func, const char *file, int line,
		 const void *actual, const void *expected, const char *msg);

#define fcheck_ptr_m(log, actual, expected, msg)\
	echeck_ptr_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_ptr_m(actual, expected, msg)\
	echeck_ptr_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_ptr(log, actual, expected)\
	echeck_ptr_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_ptr(actual, expected)\
	echeck_ptr_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check unsigned long */
int echeck_unsigned_long_m(FILE *err, const char *func, const char *file,
			   int line, unsigned long actual,
			   unsigned long expected, const char *msg);

#define fcheck_unsigned_long_m(log, actual, expected, msg)\
	echeck_unsigned_long_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_unsigned_long_m(actual, expected, msg)\
	echeck_unsigned_long_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_unsigned_long(log, actual, expected)\
	echeck_unsigned_long_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_unsigned_long(actual, expected)\
	echeck_unsigned_long_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check unsigned int */
#define echeck_unsigned_int_m(err, func, file, line, actual, expected, msg)\
	echeck_unsigned_long_m(err, func, file, line, actual, expected, msg)

#define fcheck_unsigned_int_m(log, actual, expected, msg)\
	echeck_unsigned_int_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_unsigned_int_m(actual, expected, msg)\
	echeck_unsigned_int_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_unsigned_int(log, actual, expected)\
	echeck_unsigned_int_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_unsigned_int(actual, expected)\
	echeck_unsigned_int_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check size_t */
int echeck_size_t_m(FILE *err, const char *func, const char *file, int line,
		    size_t actual, size_t expected, const char *msg);

#define fcheck_size_t_m(log, actual, expected, msg)\
	echeck_size_t_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define check_size_t_m(actual, expected, msg)\
	echeck_size_t_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, msg)

#define fcheck_size_t(log, actual, expected)\
	echeck_size_t_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

#define check_size_t(actual, expected)\
	echeck_size_t_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, expected, #actual)

/* check byte_array */
int echeck_byte_array_m(FILE *err, const char *func, const char *file, int line,
			const unsigned char *actual, size_t actual_len,
			const unsigned char *expected, size_t expected_len,
			const char *msg);

#define fcheck_byte_array_m(log, actual, actual_len, expected, expected_len,\
	msg)\
	echeck_byte_array_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, actual_len, expected, expected_len, msg)

#define check_byte_array_m(actual, actual_len, expected, expected_len, msg)\
	echeck_byte_array_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, actual_len, expected, expected_len, msg)

#define fcheck_byte_array(log, actual, actual_len, expected, expected_len)\
	echeck_byte_array_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, actual_len, expected, expected_len, #actual)

#define check_byte_array(actual, actual_len, expected, expected_len)\
	echeck_byte_array_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
		 actual, actual_len, expected, expected_len, #actual)

/* check double */
int echeck_double_m(FILE *err, const char *func, const char *file, int line,
		    double actual, double expected, double epsilon,
		    const char *msg);

#define fcheck_double_m(log, actual, expected, epsilon, msg)\
	echeck_double_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
			actual, expected, epsilon, msg)

#define check_double_m(actual, expected, epsilon, msg)\
	echeck_double_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
			actual, expected, epsilon, msg)

#define fcheck_double(log, actual, expected, epsilon)\
	echeck_double_m(log, ECHECK_FUNC, __FILE__, __LINE__,\
			actual, expected, epsilon, #actual)

#define check_double(actual, expected, epsilon)\
	echeck_double_m(stderr, ECHECK_FUNC, __FILE__, __LINE__,\
			actual, expected, epsilon, #actual)

#define echeck_double_scaled_epsilon_m(err, func, file, line, actual,\
				       expected, msg)\
	echeck_double_m(err, func, file, line, actual, expected,\
			((expected) * DBL_EPSILON), msg)

/*
 * WARNING: The "check_double_scaled_epsilon" family of functions
 * uses the "expected" value to scale DBL_EPSILON. This should not
 * be used if "expected" is an expression with side-effects.  */
#define fcheck_double_scaled_epsilon_m(log, actual, expected, msg)\
	fcheck_double_m(log, actual, expected,\
			((expected) * DBL_EPSILON), msg);

#define check_double_scaled_epsilon_m(actual, expected, msg)\
	check_double_m(actual, expected, ((expected) * DBL_EPSILON), msg);

#define fcheck_double_scaled_epsilon(log, actual, expected)\
	fcheck_double(log, actual, expected, ((expected) * DBL_EPSILON));

#define check_double_scaled_epsilon(actual, expected)\
	check_double(actual, expected, ((expected) * DBL_EPSILON));

/*check status*/
/* safe casting of non-zero int to avoid EXIT_SUCCESS */
char echeck_status_m(FILE *err, const char *func, const char *file, int line,
		     int val, const char *msg);

#define fcheck_status_m(log, val, msg)\
	echeck_status_m(log, ECHECK_FUNC, __FILE__, __LINE__, val, msg)

#define check_status_m(val, msg)\
	echeck_status_m(stderr, ECHECK_FUNC, __FILE__, __LINE__, val, msg)

#define fcheck_status(log, val)\
	echeck_status_m(log, ECHECK_FUNC, __FILE__, __LINE__, val, NULL)

#define check_status(val)\
	echeck_status_m(stderr, ECHECK_FUNC, __FILE__, __LINE__, val, NULL)

Echeck_end_C_functions
#undef Echeck_begin_C_functions
#undef Echeck_end_C_functions
#endif /* ECHECK_H */
