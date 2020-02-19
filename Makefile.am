# SPDX-License-Identifier: LGPL-3.0-or-later
# Makefile.am
# Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org>

# for environments that do not have alloca, malloc is used
# malloc instead of alloca can be forced: #define EBA_NO_ALLOCA 1
# TODO: make this plugable for non-alloca embedded environments
#ALLOCA_CFLAGS=-DEBA_NO_ALLOCA=1
ALLOCA_CFLAGS=

CSTD_CFLAGS=-std=c89

if DEBUG
BUILD_TYPE_CFLAGS=-ggdb -O0 \
	-DEBA_DIY_MEMSET=1 \
	-DEBA_DIY_MEMCPY=1 \
	-fsanitize=address,leak,undefined \
	-fno-inline-small-functions \
	-fkeep-inline-functions \
	-fkeep-static-functions \
	--coverage
BUILD_TYPE_LDFLAGS=--coverage
else
BUILD_TYPE_CFLAGS=-ggdb -O2 -DNDEBUG -Wno-unused-parameter -fomit-frame-pointer
BUILD_TYPE_LDFLAGS=
endif

#-DEBA_NO_ALLOCA=1 \
BUILD_FAUX_EMBED_CFLAGS=-ggdb -O2 -fomit-frame-pointer \
 -DNDEBUG \
 -DEBA_SKIP_LIBC=1 \
 -DEBA_CHAR_BIT=8 \
 -DEBA_DIY_MEMCPY=1 \
 -DEBA_DIY_MEMSET=1 \
 -DEBA_SKIP_EBA_NEW=1 \
 -DEBA_SKIP_TO_STRING=1 \
 -DEBA_SKIP_ENDIAN=1

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
 src/eba-internal.h \
 src/eba.c

libeba_la_LIBADD=
AM_LDFLAGS=-rdynamic $(BUILD_TYPE_LDFLAGS)

TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-get \
 test-set \
 test-set-all \
 test-swap \
 test-shift-left \
 test-shift-right \
 test-shift-fill \
 test-ring-shift \
 test-new \
 test-to-string \
 test-toggle

COMMON_TEST_SOURCES=\
 src/eba.h \
 src/eba-internal.h \
 tests/echeck.h \
 tests/echeck.c \
 tests/eba-test-private-utils.h

TEST_LDADDS=-leba

test_set_SOURCES=tests/test-set.c $(COMMON_TEST_SOURCES)
test_set_LDADD=$(TEST_LDADDS)

test_get_SOURCES=tests/test-get.c $(COMMON_TEST_SOURCES)
test_get_LDADD=$(TEST_LDADDS)

test_set_all_SOURCES=tests/test-set-all.c $(COMMON_TEST_SOURCES)
test_set_all_LDADD=$(TEST_LDADDS)

test_swap_SOURCES=tests/test-swap.c $(COMMON_TEST_SOURCES)
test_swap_LDADD=$(TEST_LDADDS)

test_ring_shift_SOURCES=tests/test-ring-shift.c $(COMMON_TEST_SOURCES)
test_ring_shift_LDADD=$(TEST_LDADDS)

test_shift_fill_SOURCES=tests/test-shift-fill.c $(COMMON_TEST_SOURCES)
test_shift_fill_LDADD=$(TEST_LDADDS)

test_shift_left_SOURCES=tests/test-shift-left.c $(COMMON_TEST_SOURCES)
test_shift_left_LDADD=$(TEST_LDADDS)

test_shift_right_SOURCES=tests/test-shift-right.c $(COMMON_TEST_SOURCES)
test_shift_right_LDADD=$(TEST_LDADDS)

test_toggle_SOURCES=tests/test-toggle.c $(COMMON_TEST_SOURCES)
test_toggle_LDADD=$(TEST_LDADDS)

test_new_SOURCES=tests/test-new.c $(COMMON_TEST_SOURCES)
test_new_LDADD=$(TEST_LDADDS)

test_to_string_SOURCES=tests/test-to-string.c $(COMMON_TEST_SOURCES)
test_to_string_LDADD=$(TEST_LDADDS)

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING COPYING.LESSER

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	$(LINDENT) \
		-T size_t \
		-T FILE \
		-T eba_s \
		`find src tests demos -name '*.h' -o -name '*.c'`

sieve-of-eratosthenes: $(libeba_la_SOURCES) demos/sieve-of-eratosthenes.c
	$(CC) $(CSTD_CFLAGS) $(BUILD_TYPE_CFLAGS) $(NOISY_CFLAGS) \
		-DEBA_SKIP_ENDIAN=1 \
		-DEBA_SKIP_SHIFTS=1 \
		-DEBA_SKIP_TO_STRING=1 \
		-DEBA_SKIP_TOGGLE=1 \
		-DEBA_SKIP_SWAP=1 \
		-o sieve-of-eratosthenes \
		-I./src/ \
		$(libeba_la_SOURCES) \
		demos/sieve-of-eratosthenes.c

demo: sieve-of-eratosthenes
	./sieve-of-eratosthenes 50

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd


vg-test-get: test-get
	./libtool --mode=execute valgrind -q ./test-get

vg-test-set: test-set
	./libtool --mode=execute valgrind -q ./test-set

vg-test-set-all: test-set-all
	./libtool --mode=execute valgrind -q ./test-set-all

vg-test-swap: test-swap
	./libtool --mode=execute valgrind -q ./test-swap

vg-test-shift-left: test-shift-left
	./libtool --mode=execute valgrind -q ./test-shift-left

vg-test-shift-right: test-shift-right
	./libtool --mode=execute valgrind -q ./test-shift-right

vg-test-shift-fill: test-shift-fill
	./libtool --mode=execute valgrind -q ./test-shift-fill

vg-test-ring-shift: test-ring-shift
	./libtool --mode=execute valgrind -q ./test-ring-shift

vg-test-toggle: test-toggle
	./libtool --mode=execute valgrind -q ./test-toggle

vg-test-new: test-new
	./libtool --mode=execute valgrind -q ./test-new

vg-test-to-string: test-to-string
	./libtool --mode=execute valgrind -q ./test-to-string

valgrind: vg-test-get vg-test-set vg-test-set-all vg-test-swap \
		vg-test-shift-left vg-test-shift-right vg-test-shift-fill \
		vg-test-ring-shift vg-test-toggle vg-test-new vg-test-to-string
