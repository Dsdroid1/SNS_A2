#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

// Function declaration
void KeyGen(int bits);
int PrimalityTest(mpz_t num);

// Driver program
void main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <key_size>\n", argv[0]);
        return;
    }
    else
    {
        int bits;
        // printf("Enter the total key size(from which prime p,q are to be choosen):");
        // scanf("%d", &bits);
        bits = atoi(argv[1]);
        KeyGen(bits);
        printf("Done!\n");
    }
}

// Function definitions
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
