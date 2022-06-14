# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org>
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libeba], [3.0.0], [eric@freesa.org])
AC_CONFIG_SRCDIR([src/eba.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S

# Checks for libraries.
# AC_CHECK_LIB([echeck], [check_str_m])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atoi])

# Add an --enable-debug option in a somewhat horrible and non-autotoolsy way
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],
		[enable debugging, default: no]),
	[case "${enableval}" in
		yes) debug=true ;;
		no)  debug=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-debug]) ;;
	 esac],
	[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_ARG_ENABLE(faux-freestanding,
	AS_HELP_STRING([--enable-faux-freestanding],
		[enable functions to support freestanding, default: no]),
	[case "${enableval}" in
		yes) faux_freestanding=true ;;
		no)  faux_freestanding=false ;;
		*)   AC_MSG_ERROR(\
		    [bad value ${enableval} for --enable-faux-freestanding]) ;;
	 esac],
	[faux_freestanding=false])
AM_CONDITIONAL(FAUX_FREESTANDING, test x"$faux_freestanding" = x"true")

AC_ARG_ENABLE(skip-eab-new,
	AS_HELP_STRING([--enable-skip-eab-new],
		[enable skipping of eab_new constructor code, default: no]),
	[case "${enableval}" in
		yes) skip_eab_new=true ;;
		no)  skip_eab_new=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-eab-new]) ;;
	esac],
	[skip_eab_new=false])
AM_CONDITIONAL(SKIP_EBA_NEW, test x"$skip_eab_new" = x"true")

AC_ARG_ENABLE(skip-to-string,
	AS_HELP_STRING([--enable-skip-to-string],
		[enable skipping of to_string code, default: no]),
	[case "${enableval}" in
		yes) skip_to_string=true ;;
		no)  skip_to_string=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-to-string]) ;;
	esac],
	[skip_to_string=false])
AM_CONDITIONAL(SKIP_TO_STRING, test x"$skip_to_string" = x"true")

AC_ARG_ENABLE(skip-swap,
	AS_HELP_STRING([--enable-skip-swap],
		[enable skipping of swap code, default: no]),
	[case "${enableval}" in
		yes) skip_swap=true ;;
		no)  skip_swap=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-swap]) ;;
	esac],
	[skip_swap=false])
AM_CONDITIONAL(SKIP_SWAP, test x"$skip_swap" = x"true")

AC_ARG_ENABLE(skip-set-all,
	AS_HELP_STRING([--enable-skip-set-all],
		[enable skipping of set_all code, default: no]),
	[case "${enableval}" in
		yes) skip_set_all=true ;;
		no)  skip_set_all=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-set-all]) ;;
	esac],
	[skip_set_all=false])
AM_CONDITIONAL(SKIP_SET_ALL, test x"$skip_set_all" = x"true")

AC_ARG_ENABLE(skip-shifts,
	AS_HELP_STRING([--enable-skip-shifts],
		[enable skipping of shifts code, default: no]),
	[case "${enableval}" in
		yes) skip_shifts=true ;;
		no)  skip_shifts=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-shifts]) ;;
	esac],
	[skip_shifts=false])
AM_CONDITIONAL(SKIP_SHIFTS, test x"$skip_shifts" = x"true")

AC_ARG_ENABLE(skip-toggle,
	AS_HELP_STRING([--enable-skip-toggle],
		[enable skipping of toggle code, default: no]),
	[case "${enableval}" in
		yes) skip_toggle=true ;;
		no)  skip_toggle=false ;;
		*)   AC_MSG_ERROR(\
			[bad value ${enableval} for --enable-skip-toggle]) ;;
	esac],
	[skip_toggle=false])
AM_CONDITIONAL(SKIP_TOGGLE, test x"$skip_toggle" = x"true")

AM_INIT_AUTOMAKE([subdir-objects -Werror -Wall])
AM_PROG_AR
LT_INIT

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
