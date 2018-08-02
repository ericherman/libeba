# Makefile.am
# Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>
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

# for environments that do not have alloca, malloc is used
# malloc instead of alloca can be forced: #define EBA_NO_ALLOCA 1
# TODO: make this plugable for non-alloca embedded environments
#ALLOCA_CFLAGS=-DEBA_NO_ALLOCA=1
ALLOCA_CFLAGS=

CSTD_CFLAGS=-std=c89

BUILD_NORMAL_CFLAGS=-ggdb -O2 -fomit-frame-pointer \
 -DNDEBUG -Wno-unused-parameter

BUILD_DEBUG_CFLAGS=-ggdb -O0 -Wno-unused-parameter

BUILD_FAUX_EMBED_CFLAGS=-ggdb -O2 -fomit-frame-pointer \
 -DNDEBUG -Wno-unused-parameter \
 -DEBA_CHAR_BIT=8 \
 -DEBA_DIY_MEMCPY \
 -DEBA_NO_ALLOCA \
 -DEBA_SKIP_EBA_NEW \
 -DEBA_SKIP_LOG_ERRORS \
 -DEBA_SKIP_INCLUDE_STDDEF -DEBA_SIZE_TYPE="unsigned long int" \
 -DEBA_SKIP_ENDIAN \
 -DEBA_SKIP_STRUCT_NULL_CHECK \
 -DEBA_SKIP_STRUCT_BITS_NULL_CHECK \
 -DEBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY

#BUILD_TYPE_CFLAGS=$(BUILD_NORMAL_CFLAGS)
BUILD_TYPE_CFLAGS=$(BUILD_FAUX_EMBED_CFLAGS)

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

AM_CFLAGS=$(CSTD_CFLAGS) \
 $(ALLOCA_CFLAGS) \
 $(BUILD_TYPE_CFLAGS) \
 $(NOISY_CFLAGS) \
 -pipe

lib_LTLIBRARIES=libeba.la
include_HEADERS=src/eba.h

libeba_la_SOURCES=\
 src/eba.h \
 src/eba.c

libeba_la_LIBADD=
AM_LDFLAGS=-rdynamic

TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-get \
 test-set \
 test-swap \
 test-shift-fill \
 test-ring-shift \
 test-toggle

COMMON_TEST_SOURCES=src/eba.h \
 tests/echeck.h \
 tests/echeck.c \
 tests/eba-test-private-utils.h

TEST_LDADDS=-leba

test_set_SOURCES=tests/test-set.c $(COMMON_TEST_SOURCES)
test_set_LDADD=$(TEST_LDADDS)

test_get_SOURCES=tests/test-get.c $(COMMON_TEST_SOURCES)
test_get_LDADD=$(TEST_LDADDS)

test_swap_SOURCES=tests/test-swap.c $(COMMON_TEST_SOURCES)
test_swap_LDADD=$(TEST_LDADDS)

test_ring_shift_SOURCES=tests/test-ring-shift.c $(COMMON_TEST_SOURCES)
test_ring_shift_LDADD=$(TEST_LDADDS)

test_shift_fill_SOURCES=tests/test-shift-fill.c $(COMMON_TEST_SOURCES)
test_shift_fill_LDADD=$(TEST_LDADDS)

test_toggle_SOURCES=tests/test-toggle.c $(COMMON_TEST_SOURCES)
test_toggle_LDADD=$(TEST_LDADDS)

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.GPL3 misc

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

sieve-of-eratosthenes:
	$(CC) $(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS) \
		-Wno-unused-parameter \
		-DEBA_SKIP_ENDIAN \
		-DEBA_SKIP_SHIFTS \
		-DEBA_SKIP_STRUCT_NULL_CHECK \
		-DEBA_SKIP_STRUCT_BITS_NULL_CHECK \
		-DEBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY \
		-o sieve-of-eratosthenes \
		$(libeba_la_SOURCES) \
		demos/sieve-of-eratosthenes.c

demo: sieve-of-eratosthenes
	./sieve-of-eratosthenes 50

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd

valgrind: $(check_PROGRAMS)
	libtool --mode=execute valgrind -q ./test-get
	libtool --mode=execute valgrind -q ./test-set
	libtool --mode=execute valgrind -q ./test-swap
	libtool --mode=execute valgrind -q ./test-shift-fill
	libtool --mode=execute valgrind -q ./test-ring-shift
	libtool --mode=execute valgrind -q ./test-toggle
