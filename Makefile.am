# Makefile.am
# Copyright (C) 2017 Eric Herman <eric@freesa.org>
#
# This work is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This work is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

lib_LTLIBRARIES=libeba.la
include_HEADERS=src/eba.h

libeba_la_SOURCES=\
 src/eba.h \
 src/eba.c \
 src/eba-log.h \
 src/eba-log.c

libeba_la_LIBADD=
AM_LDFLAGS=-rdynamic

TESTS=$(check_PROGRAMS)
check_PROGRAMS=

#COMMON_TEST_SOURCES=src/eba.h

TEST_LDADDS=-leba

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.GPL3 misc

CSTD_CFLAGS=-std=c89

DEBUG_CFLAGS=-ggdb -O2
#DEBUG_CFLAGS=-ggdb -O0

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

AM_CFLAGS=$(CSTD_CFLAGS) \
 $(DEBUG_CFLAGS) \
 $(NOISY_CFLAGS)

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	patch -Np1 -i misc/pre-tidy.patch
	$(LINDENT) \
		-T size_t \
		-T FILE \
		-T eba_s \
		`find src tests demos -name '*.h' -o -name '*.c'`
	patch -Rp1 -i misc/pre-tidy.patch

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
