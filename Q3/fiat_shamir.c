#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include "fiat_shamir.h"

int PrimalityTest(mpz_t num)
{
    mpz_t i, sqrt_num, mod_value;
    mpz_init(i);
    mpz_init(sqrt_num);
    mpz_init(mod_value);
    int answer = 1;

    mpz_set_ui(i, 2);
    mpz_mod(mod_value, num, i);
    if (mpz_cmp_ui(mod_value, 0) == 0)
    {
        answer = 0;
    }
    else
    {
        mpz_sqrt(sqrt_num, num);
        mpz_add_ui(i, i, 1);
        for (; mpz_cmp(i, sqrt_num) <= 0 && answer == 1; mpz_add_ui(i, i, 2))
        {
            mpz_mod(mod_value, num, i);
            if (mpz_cmp_ui(mod_value, 0) == 0)
            {
                answer = 0;
            }
        }
    }

    mpz_clear(i);
    mpz_clear(sqrt_num);
    mpz_clear(mod_value);
    return answer;
}

void KeyGen(int bits)
{
    // Choose 2 large primes
    mpz_t p, q, n;
    mpz_inits(p, q, n, NULL);

    // GENERATE a random number
    gmp_randstate_t rstate;
    gmp_randinit_default(rstate);
    do
    {
        mpz_urandomb(p, rstate, bits);
        // gmp_printf("DEBUG:%Zd\n", p);
    } while (PrimalityTest(p) == 0);
    do
    {
        mpz_urandomb(q, rstate, bits);
        // gmp_printf("DEBUG:%Zd\n", q);
    } while (PrimalityTest(q) == 0);
    gmp_printf("Primes p:%Zd, q:%Zd\n", p, q);
    // Calculate n = pq
    mpz_mul(n, p, q);

    // Get a secret key for Alice
    mpz_t S, V;
    mpz_inits(S, V, NULL);

    do
    {
        mpz_urandomm(S, rstate, n);
    } while (mpz_cmp_ui(S, 0) == 0 || mpz_cmp(S, p) == 0 || mpz_cmp(S, q) == 0);

    // Calculate V = S^2 mod n
    mpz_mul(V, S, S);
    mpz_mod(V, V, n);

    // Store keys in file
    FILE *fp;
    fp = fopen("PK.txt", "w");
    if (fp != NULL)
    {
        mpz_out_str(fp, 10, n);
        fprintf(fp, "\n");
        mpz_out_str(fp, 10, V);
    }
    else
    {
        // Error occured in creating file
    }
    fclose(fp);
    fp = fopen("SK.txt", "w");
    if (fp != NULL)
    {
        mpz_out_str(fp, 10, n);
        fprintf(fp, "\n");
        mpz_out_str(fp, 10, S);
    }
    fclose(fp);

    mpz_clears(S, V, NULL);
    mpz_clears(p, q, n, NULL);
    gmp_randclear(rstate);
}

int ReadPK(mpz_t n, mpz_t V)
{
    FILE *fp;
    int retval = 1;
    fp = fopen("PK.txt", "r");
    if (fp != NULL)
    {
        mpz_inp_str(n, fp, 10);
        mpz_inp_str(V, fp, 10);
    }
    else
    {
        retval = -1;
    }
    return retval;
}

int ReadSK(mpz_t n, mpz_t S)
{
    FILE *fp;
    int retval = 1;
    fp = fopen("SK.txt", "r");
    if (fp != NULL)
    {
        mpz_inp_str(n, fp, 10);
        mpz_inp_str(S, fp, 10);
    }
    else
    {
        retval = -1;
    }
    return retval;
}

void choose_witness(mpz_t n, mpz_t r, mpz_t x)
{
    // Choose random r
    // GENERATE a random number
    gmp_randstate_t rstate;
    gmp_randinit_default(rstate);
    static time_t seed;
    seed += 1;
    gmp_randseed_ui(rstate, seed);
    do
    {
        mpz_urandomm(r, rstate, n);
    } while (mpz_cmp_ui(r, 0) == 0);

    // Calculate witness x= r^2 mod n
    mpz_mul(x, r, r);
    mpz_mod(x, x, n);

    gmp_randclear(rstate);
}

int choose_challenge()
{
    srand(time(0));
    int c = rand() % 2;
    return c;
}

void compute_response(mpz_t n, mpz_t r, mpz_t S, int c, mpz_t y)
{
    mpz_t power;
    mpz_init(power);

    mpz_set_ui(power, c);
    mpz_powm(y, S, power, n);
    mpz_mul(y, y, r);
    mpz_mod(y, y, n);

    mpz_clear(power);
}

int verify_response(mpz_t n, mpz_t x, mpz_t V, int c, mpz_t y)
{
    mpz_t power, y_square, rhs;
    int retval;
    mpz_inits(power, y_square, rhs, NULL);

    mpz_set_ui(power, c);

    // Calculate response^2
    mpz_mul(y_square, y, y);
    mpz_mod(y_square, y_square, n);

    // Calculate x*V^c
    mpz_powm(rhs, V, power, n);
    mpz_mul(rhs, rhs, x);
    mpz_mod(rhs, rhs, n);

    gmp_printf("Value of y^2 mod n:%Zd\n", y_square);
    gmp_printf("Value of x*V^c mod n:%Zd\n", rhs);

    if (mpz_cmp(y_square, rhs) == 0)
    {
        retval = 1;
        // Verified
    }
    else
    {
        retval = -1;
        // Verification unsuccessful
    }

    mpz_clears(power, y_square, rhs, NULL);
    return retval;
}

// void main()
// {
//     // KeyGen(10);
//     mpz_t n, S, V, x, r, y;
//     mpz_inits(n, S, V, x, r, y, NULL);

//     ReadPK(n, V);
//     gmp_printf("PK read: n:%Zd, V:%Zd\n", n, V);
//     ReadSK(n, S);
//     gmp_printf("SK read: n:%Zd, S:%Zd\n", n, S);

//     // witness
//     choose_witness(n, r, x);
//     gmp_printf("r:%Zd, x:%Zd\n", r, x);
//     int c = choose_challenge();
//     printf("Challenge c:%d\n", c);
//     compute_response(n, r, S, c, y);
//     gmp_printf("Response y:%Zd\n", y);
//     int v = verify_response(n, x, V, c, y);
//     printf("Validity: %d\n", v);
//     mpz_clears(n, S, V, x, r, y, NULL);
// }