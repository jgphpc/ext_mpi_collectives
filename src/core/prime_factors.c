#include "prime_factors.h"
#include <math.h>
#include <stdlib.h>

struct prime_factors {
  int prime;
  int count;
};

static void prime_rost(int max_number, int *primes) {
  int i, j;
  for (i = 0; i < max_number; i++) {
    primes[i] = 1;
  }
  for (i = 2; i < max_number; i++) {
    if (primes[i]) {
      for (j = 2; i * j < max_number; j++) {
        primes[i * j] = 0;
      }
    }
  }
}

static int prime_factor_decomposition(int number,
                                      struct prime_factors *factors) {
  int primes[number + 2], max_factor, i, j;
  for (i = 0; i < number + 1; i++) {
    factors[i].count = factors[i].prime = 0;
  }
  prime_rost(number + 1, primes);
  max_factor = 0;
  for (i = 2; i < number + 1; i++) {
    if (primes[i]) {
      factors[max_factor++].prime = i;
    }
  }
  for (i = 0; i < max_factor; i++) {
    j = number;
    while (j % factors[i].prime == 0) {
      j /= factors[i].prime;
      factors[i].count++;
    }
  }
  return (max_factor);
}

static int plain_prime_factors(int number, int *prime_factors) {
  struct prime_factors factors[number + 1];
  int primes_max, i, j, k;
  primes_max = prime_factor_decomposition(number, factors);
  for (i = k = 0; i < primes_max; i++) {
    for (j = 0; j < factors[i].count; j++) {
      prime_factors[k++] = factors[i].prime;
    }
  }
  prime_factors[k] = 0;
  return (k);
}

static int factors_minimum_compare(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

static int factors_minimum_compare_reverse(const void *a, const void *b) {
  return (*(int *)b - *(int *)a);
}

int ext_mpi_factors_minimum(int number, int factor_min, int *factors) {
  int factors_max, i;
  factors_max = plain_prime_factors(number, factors);
  while ((factors[0] < factor_min) && (factors_max >= 2)) {
    factors[0] *= factors[1];
    factors_max--;
    for (i = 1; i <= factors_max; i++) {
      factors[i] = factors[i + 1];
    }
    qsort(factors, factors_max, sizeof(int), factors_minimum_compare);
  }
  if (factors_max >= 2) {
    qsort(factors, factors_max, sizeof(int), factors_minimum_compare_reverse);
  }
  return (factors_max);
}

int ext_mpi_factor_sqrt(int number) {
  int factors[number], factors_max, i;
  factors_max = plain_prime_factors(number, factors);
  while ((factors[0] < sqrt(number)) && (factors_max >= 2)) {
    factors[0] *= factors[1];
    factors_max--;
    for (i = 1; i <= factors_max; i++) {
      factors[i] = factors[i + 1];
    }
  }
  return (factors[0]);
}
