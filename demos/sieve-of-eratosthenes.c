/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* sieve-of-eratosthenes.c: demo of using libeba embedable bit array */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include <stdio.h>
#include <stdlib.h>

#include "../src/eba.h"

int main(int argc, char **argv)
{
	unsigned long max;
	struct eba_s *eba;
	size_t i, j;

	max = argc > 1 ? strtoul(argv[1], NULL, 10) : 100;

	/* array size is (max+1) because of zero offset */
	eba = eba_new(max + 1);
	if (!eba) {
		fprintf(stderr, "eba_new returned NULL\n");
		return 1;
	}

	/* but start with guessing everything else is prime */
	eba_set_all(eba, 0x01);

	/* zero and one are not prime */
	eba_set(eba, 0, 0);
	eba_set(eba, 1, 0);

	/* set all multiples of 2 as non-prime */
	for (i = 4; (i * 2) <= max; i += 2) {
		eba_set(eba, i, 0);
	}

	/* now walk the odd numbers in the array looking for primes */
	for (i = 3; (i * 2) <= max; i += 1) {
		/* if prime */
		if (eba_get(eba, i)) {
			/* mark all multiples as non-prime */
			for (j = i + i; j <= max; j += i) {
				eba_set(eba, j, 0);
			}
		}
	}

	printf("Prime numbers up to %lu:\n", (unsigned long)max);
	for (i = 0; i <= max; ++i) {
		if (eba_get(eba, i)) {
			printf("%lu\n", (unsigned long)i);
		}
	}

	eba_free(eba);
	return 0;
}
