# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org>
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libeba], [2.0.2], [eric@freesa.org])
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
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([atoi])

# Add an --enable-debug option in a somewhat horrible and non-autotoolsy way
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AM_INIT_AUTOMAKE([subdir-objects -Werror -Wall])
AM_PROG_AR
LT_INIT

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
