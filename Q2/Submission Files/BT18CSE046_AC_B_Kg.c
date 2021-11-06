#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmp.h>

// Function Prototypes
void KeyGen(int bits);
int PrimalityTest(mpz_t num);

// Driver program
void main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <keysize>", argv[0]);
    }
    else
    {
        int bits;
        bits = atoi(argv[1]);
        KeyGen(bits);
        printf("Done!\n");
    }
}

// Function Definitions
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