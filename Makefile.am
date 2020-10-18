# SPDX-License-Identifier: LGPL-3.0-or-later
# Makefile.am
# Copyright (C) 2017, 2018, 2019 Eric Herman <eric@freesa.org>


CSTD_CFLAGS=-std=c89

if DEBUG
BUILD_TYPE_CFLAGS=-ggdb -O0 \
	-fsanitize=address,leak,undefined \
	-fno-inline-small-functions \
	-fkeep-inline-functions \
	-fkeep-static-functions \
	--coverage
BUILD_TYPE_LDFLAGS=--coverage
else
BUILD_TYPE_CFLAGS=-ggdb -O2 -DNDEBUG -fomit-frame-pointer
endif

if NO_ALLOCA
EAB_NO_ALLOCA_CFLAGS=-DEBA_NO_ALLOCA=1
endif

if SKIP_ALLOCA_H
EBA_SKIP_ALLOCA_H_CFLAGS=-DSKIP_ALLOCA_H=1
endif

if FAUX_FREESTANDING
EBA_HOSTED_CFLAGS=-DEBA_HOSTED=0 -DFAUX_FREESTANDING=1
endif

if SKIP_EBA_NEW
EBA_SKIP_EBA_NEW_CFLAGS=-DEBA_SKIP_EBA_NEW=1
endif

if SKIP_TO_STRING
EBA_SKIP_TO_STRING_CFLAGS=-DEBA_SKIP_TO_STRING=1
endif

if SKIP_SWAP
EBA_SKIP_SWAP_CFLAGS=-DEBA_SKIP_SWAP=1
endif

if SKIP_SET_ALL
EBA_SKIP_SET_ALL_CFLAGS=-DEBA_SKIP_SET_ALL=1
endif

if SKIP_SHIFTS
EBA_SKIP_SHIFTS_CFLAGS=-DEBA_SKIP_SHIFTS=1
endif

if SKIP_TOGGLE
EBA_SKIP_TOGGLE_CFLAGS=-DEBA_SKIP_TOGGLE=1
endif

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

AM_CFLAGS=$(CSTD_CFLAGS) \
 $(BUILD_TYPE_CFLAGS) \
 $(EAB_NO_ALLOCA_CFLAGS) \
 $(EAB_SKIP_ALLOCA_H_CFLAGS) \
 $(EBA_HOSTED_CFLAGS) \
 $(EBA_SKIP_EBA_NEW_CFLAGS) \
 $(EBA_SKIP_TO_STRING_CFLAGS) \
 $(EBA_SKIP_SWAP_CFLAGS) \
 $(EBA_SKIP_SET_ALL_CFLAGS) \
 $(EBA_SKIP_SHIFTS_CFLAGS) \
 $(EBA_SKIP_TOGGLE_CFLAGS) \
 $(NOISY_CFLAGS) \
 -pipe

lib_LTLIBRARIES=libeba.la
include_HEADERS=src/eba.h

libeba_la_SOURCES=\
 src/eba.h \
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
 test-rotate \
 test-new \
 test-to-string \
 test-toggle

COMMON_TEST_SOURCES=\
 src/eba.h \
 submodules/libecheck/src/echeck.h \
 submodules/libecheck/src/echeck.c \
 tests/eba-test-private-utils.h

TEST_LDADDS=-leba
TEST_CFLAGS=-I ./submodules/libecheck/src -I ./src

test_set_SOURCES=tests/test-set.c $(COMMON_TEST_SOURCES)
test_set_LDADD=$(TEST_LDADDS)
test_set_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_get_SOURCES=tests/test-get.c $(COMMON_TEST_SOURCES)
test_get_LDADD=$(TEST_LDADDS)
test_get_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_set_all_SOURCES=tests/test-set-all.c $(COMMON_TEST_SOURCES)
test_set_all_LDADD=$(TEST_LDADDS)
test_set_all_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_swap_SOURCES=tests/test-swap.c $(COMMON_TEST_SOURCES)
test_swap_LDADD=$(TEST_LDADDS)
test_swap_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_rotate_SOURCES=tests/test-rotate.c $(COMMON_TEST_SOURCES)
test_rotate_LDADD=$(TEST_LDADDS)
test_rotate_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_shift_fill_SOURCES=tests/test-shift-fill.c $(COMMON_TEST_SOURCES)
test_shift_fill_LDADD=$(TEST_LDADDS)
test_shift_fill_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_shift_left_SOURCES=tests/test-shift-left.c $(COMMON_TEST_SOURCES)
test_shift_left_LDADD=$(TEST_LDADDS)
test_shift_left_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_shift_right_SOURCES=tests/test-shift-right.c $(COMMON_TEST_SOURCES)
test_shift_right_LDADD=$(TEST_LDADDS)
test_shift_right_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_toggle_SOURCES=tests/test-toggle.c $(COMMON_TEST_SOURCES)
test_toggle_LDADD=$(TEST_LDADDS)
test_toggle_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_new_SOURCES=tests/test-new.c $(COMMON_TEST_SOURCES)
test_new_LDADD=$(TEST_LDADDS)
test_new_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

test_to_string_SOURCES=tests/test-to-string.c $(COMMON_TEST_SOURCES)
test_to_string_LDADD=$(TEST_LDADDS)
test_to_string_CFLAGS=$(AM_CFLAGS) $(TEST_CFLAGS)

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING COPYING.LESSER \
	demos/sieve-of-eratosthenes.c \
	submodules/libecheck/COPYING \
	submodules/libecheck/COPYING.LESSER \
	submodules/libecheck/src/echeck.h \
	submodules/libecheck/src/echeck.c

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	$(LINDENT) \
		-T size_t \
		-T FILE \
		`find src tests demos -name '*.h' -o -name '*.c'` \
		eba_test_arduino/eba_test_arduino.ino

sieve-of-eratosthenes: $(libeba_la_SOURCES) demos/sieve-of-eratosthenes.c
	$(CC) $(CSTD_CFLAGS) $(BUILD_TYPE_CFLAGS) $(NOISY_CFLAGS) \
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
	pushd demos && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd submodules/libecheck && \
		rm -rf `cat ../../.gitignore | sed -e 's/#.*//'` \
		rm -rf `cat .gitignore | sed -e 's/#.*//'`; \
		popd


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

vg-test-rotate: test-rotate
	./libtool --mode=execute valgrind -q ./test-rotate

vg-test-toggle: test-toggle
	./libtool --mode=execute valgrind -q ./test-toggle

vg-test-new: test-new
	./libtool --mode=execute valgrind -q ./test-new

vg-test-to-string: test-to-string
	./libtool --mode=execute valgrind -q ./test-to-string

valgrind: vg-test-get vg-test-set vg-test-set-all vg-test-swap \
		vg-test-shift-left vg-test-shift-right vg-test-shift-fill \
		vg-test-rotate vg-test-toggle vg-test-new vg-test-to-string
	@echo valgrind ok
