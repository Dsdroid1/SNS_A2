#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <stdio.h>
#include <gmp.h>

void KeyGen(int bits);
int ReadPK(mpz_t q, mpz_t g1, mpz_t g);
int ReadSK(mpz_t q, mpz_t x);
void Encrypt(mpz_t data, mpz_t q, mpz_t g, mpz_t g1, mpz_t c1, mpz_t c2);
void Decrypt(mpz_t c1, mpz_t c2, mpz_t q, mpz_t x, mpz_t PT);

#endif