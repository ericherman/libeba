/*
sieve-of-eratosthenes.c: demo of using libeba embedable bit array
Copyright (C) 2017 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/

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
	/* any endian-ness is fine: eba->bytes will not be used elsewhere */
	eba = eba_new(max + 1, eba_endian_little);
	if (!eba) {
		fprintf(stderr, "eba_new returned NULL\n");
		return 1;
	}

	/* zero and one are not prime */
	eba_set(eba, 0, 0);
	eba_set(eba, 1, 0);

	/* but start with guessing everything else is prime */
	for (i = 2; i <= max; ++i) {
		eba_set(eba, i, 1);
	}

	/* now walk the array looking for primes */
	for (i = 2; (i * 2) <= max; ++i) {
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
