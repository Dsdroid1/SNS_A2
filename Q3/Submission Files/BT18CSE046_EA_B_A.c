#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <gmp.h>
#include <time.h>

// Function declarations
int ReadSK(mpz_t n, mpz_t S);
void choose_witness(mpz_t n, mpz_t r, mpz_t x);
void compute_response(mpz_t n, mpz_t r, mpz_t S, int c, mpz_t y);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }
    else
    {
        WSADATA wsaData = {0};
        SOCKET sock;
        SOCKADDR_IN SockAddr = {0};
        int port = 9001;

        WSAStartup(MAKEWORD(2, 2), &wsaData);

        sock = socket(AF_INET, SOCK_STREAM, 0);

        SockAddr.sin_family = AF_INET;
        SockAddr.sin_port = htons(port);
        SockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock, (SOCKADDR *)&SockAddr, sizeof(SockAddr)) != SOCKET_ERROR)
        {
            char buf[5000];
            printf("Conn established!\n");
            // Init the SK
            mpz_t n, S, x, r, y;
            mpz_inits(n, S, x, r, y, NULL);

            if (ReadSK(n, S) < 0)
            {
                // Keys not available, terminating
                printf("Keys not avaialable!\n");
            }
            else
            {
                int num_rounds = 3, curr_round = 1;
                while (curr_round <= num_rounds)
                {
                    printf("Round %d\n", curr_round);
                    // Choose the witness x
                    choose_witness(n, r, x);
                    // Send the witness to Bob
                    char *witness;
                    witness = mpz_get_str(NULL, 10, x);
                    gmp_printf("Generated witness x:%Zd\n", x);
                    send(sock, witness, strlen(witness), 0);
                    printf("Successfully sent witness x to Bob\n");
                    char buf[2];
                    buf[1] = '\0';
                    int iRet = recv(sock, buf, 1, 0);
                    if (iRet != SOCKET_ERROR)
                    {
                        printf("Bob sent challenge:%s\n", buf);
                        int c = buf[0] - '0';
                        // Generate response y
                        compute_response(n, r, S, c, y);
                        gmp_printf("Computed response y:%Zd\n", y);
                        char *response;
                        response = mpz_get_str(NULL, 10, y);
                        // Send response to Bob
                        send(sock, response, strlen(response), 0);
                        printf("Successfully sent response y to Bob\n");
                    }
                    else
                    {
                        // Some error occured, terminate
                        printf("Did not receive challenge from Bob successfully, terminating...\n");
                    }
                    free(witness);
                    curr_round++;
                }
            }

            mpz_clears(n, S, x, r, y, NULL);
        }
        else
        {
            printf("Error");
        }
        closesocket(sock);

        WSACleanup();
    }

    return 0;
}

// Function definitions
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