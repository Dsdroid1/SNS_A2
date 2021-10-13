#ifndef FIAT_SHAMIR_H
#define FIAT_SHAMIR_H

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

void KeyGen(int bits);
int ReadPK(mpz_t n, mpz_t V);
int ReadSK(mpz_t n, mpz_t S);
void choose_witness(mpz_t n, mpz_t r, mpz_t x);
int choose_challenge();
void compute_response(mpz_t n, mpz_t r, mpz_t S, int c, mpz_t y);
int verify_response(mpz_t n, mpz_t x, mpz_t V, int c, mpz_t y);

#endif