#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmp.h>

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
    // We require a large prime 'q', k bits long

    // GENERATE a random number
    gmp_randstate_t rstate;
    gmp_randinit_default(rstate);

    mpz_t num;
    mpz_init(num);
    do
    {
        mpz_urandomb(num, rstate, bits);
        // gmp_printf("DEBUG:%Zd\n", num);
    } while (PrimalityTest(num) == 0);
    gmp_printf("PRIME q:%Zd\n", num);

    mpz_t g, g1, x;

    mpz_init(g);
    mpz_init(g1);
    mpz_init(x);

    // Now we take a generator g belonging to the RRSM of q
    do
    {
        // Get random g in range [1,num-1]
        mpz_urandomm(g, rstate, num);
    } while (mpz_cmp_ui(g, 0) == 0);

    // Select random x belonging to Zq* as the secret key
    do
    {
        // Get random x in range [1,num-1]
        mpz_urandomm(x, rstate, num);
    } while (mpz_cmp_ui(x, 0) == 0);

    // Now calculate g1= g^x mod q
    mpz_powm(g1, g, x, num);

    // Store keys in file
    FILE *fp;
    fp = fopen("PK.txt", "w");
    mpz_out_str(fp, 10, num);
    fprintf(fp, "\n");
    mpz_out_str(fp, 10, g1);
    fprintf(fp, "\n");
    mpz_out_str(fp, 10, g);
    fclose(fp);
    fp = fopen("SK.txt", "w");
    mpz_out_str(fp, 10, num);
    fprintf(fp, "\n");
    mpz_out_str(fp, 10, x);
    fclose(fp);

    mpz_clear(g);
    mpz_clear(g1);
    mpz_clear(x);
    mpz_clear(num);
    gmp_randclear(rstate);
}

int ReadPK(mpz_t q, mpz_t g1, mpz_t g)
{
    FILE *fp;
    int retval = 1;
    fp = fopen("PK.txt", "r");
    if (fp != NULL)
    {
        mpz_inp_str(q, fp, 10);
        mpz_inp_str(g1, fp, 10);
        mpz_inp_str(g, fp, 10);
    }
    else
    {
        retval = -1;
    }
    fclose(fp);
    // gmp_printf("q=%Zd,g1=%Zd,g=%Zd\n", q, g1, g);
    return retval;
}

int ReadSK(mpz_t q, mpz_t x)
{
    FILE *fp;
    int retval = 1;
    fp = fopen("SK.txt", "r");
    if (fp != NULL)
    {
        mpz_inp_str(q, fp, 10);
        mpz_inp_str(x, fp, 10);
    }
    else
    {
        retval = -1;
    }
    fclose(fp);
    return retval;
}

void Encrypt(mpz_t data, mpz_t q, mpz_t g, mpz_t g1, mpz_t c1, mpz_t c2)
{
    if (mpz_cmp(data, q) < 0)
    {
        // Choose y belonging to Zq*
        mpz_t y;
        mpz_init(y);

        // GENERATE a random number
        gmp_randstate_t rstate;
        gmp_randinit_default(rstate);

        do
        {
            // Get random y in range [1,q-1]
            mpz_urandomm(y, rstate, q);
        } while (mpz_cmp_ui(y, 0) == 0);

        // Compute C2 =g^y, C1 = (g1^y)*m
        mpz_powm(c1, g1, y, q);
        mpz_mul(c1, c1, data);
        mpz_mod(c1, c1, q);

        mpz_powm(c2, g, y, q);
        // gmp_printf("C1:%Zd, C2:%Zd\n", c1, c2);
        mpz_clear(y);
        gmp_randclear(rstate);
    }
    else
    {
        // Do something
        mpz_mod(data, data, q);
        Encrypt(data, q, g, g1, c1, c2);
    }
}

void Decrypt(mpz_t c1, mpz_t c2, mpz_t q, mpz_t x, mpz_t PT)
{
    mpz_t data;
    mpz_init(data);

    // C2^x
    mpz_powm(data, c2, x, q);
    // (C2^x)^-1
    mpz_invert(data, data, q);
    // C1*(C2^x)^-1
    mpz_mul(data, data, c1);
    mpz_mod(data, data, q);
    mpz_set(PT, data);
    // gmp_printf("Data:%Zd\n", PT);

    mpz_clear(data);
}

// void main()
// {
//     // KEYGEN TEST
//     // KeyGen(5);
//     // KeyGen(10);
//     // KEY READ TEST
//     // mpz_t g, g1, q;
//     // mpz_inits(g, g1, q, NULL);
//     // ReadPK(q, g1, g);
//     // gmp_printf("q=%Zd,g1=%Zd,g=%Zd", q, g1, g);
//     // mpz_clears(g, g1, q, NULL);
//     // ENC-DEC TEST
//     mpz_t g, g1, q, x, data, c1, c2, pt;
//     mpz_inits(g, g1, q, x, data, c1, c2, pt, NULL);
//     ReadPK(q, g1, g);
//     ReadSK(q, x);
//     char str[10] = "60";
//     mpz_set_str(data, str, 10);
//     mpz_mod(data, data, q);
//     gmp_printf("Data before encryption: %Zd\n", data);
//     // Encrpyt
//     Encrpyt(data, q, g, g1, c1, c2);
//     // Decrypt
//     Decrypt(c1, c2, q, x, pt);
//     mpz_clears(g, g1, q, x, data, c1, c2, pt, NULL);
// }