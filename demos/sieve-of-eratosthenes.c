#include <stdio.h>
#include <stdlib.h>

#include "../src/eba.h"

int main(int argc, char **argv)
{
	size_t i, j, max;
	struct eba_s *eba;

	max = argc > 1 ? atoi(argv[1]) : 100;

	eba = eba_new(max + 1);

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
