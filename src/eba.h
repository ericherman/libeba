/*
eba.h: embedable bit array - hopefully somewhat suitable for small CPUs
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

#ifndef EBA_H
#define EBA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>		/* size_t */

/* embedable bit vector */
struct eba_s {
	unsigned char *bits;
	size_t size;
};

/* #define Eba_alloc and Eba_free to use something other than malloc/free */
struct eba_s *eba_new(unsigned long num_bits);

void eba_free(struct eba_s *eba);

void eba_set(struct eba_s *eba, unsigned long index, unsigned char val);

unsigned char eba_get(struct eba_s *eba, unsigned long index);

#ifdef __cplusplus
}
#endif

#endif /* EBA_H */
