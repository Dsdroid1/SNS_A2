#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <gmp.h>
// Function declarations
int ReadPK(mpz_t q, mpz_t g1, mpz_t g);
void Encrypt(mpz_t data, mpz_t q, mpz_t g, mpz_t g1, mpz_t c1, mpz_t c2);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <message(as_integer_base_10)>\n", argv[0]);
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

            mpz_t q, g, g1, data, c1, c2;
            mpz_inits(q, g, g1, data, c1, c2, NULL);

            if (ReadPK(q, g1, g) < 0)
            {
                printf("Error reading keys");
            }
            // printf("BUFFER:%s", buf);
            // gmp_printf("Enter message(Prime q=%Zd)[Message number between 1 and q-1]:", q);
            // gets(buf);
            strcpy(buf, argv[1]);
            mpz_set_str(data, buf, 10);
            mpz_mod(data, data, q); // Normalize under mod q
            Encrypt(data, q, g, g1, c1, c2);
            char *str_c1, *str_c2, *str;
            str_c1 = mpz_get_str(NULL, 10, c1);
            // send(sock, str, strlen(str), 0);
            str_c2 = mpz_get_str(NULL, 10, c2);
            // send(sock, str, strlen(str), 0);
            gmp_printf("C1:%Zd, C2:%Zd\n", c1, c2);
            str = (char *)malloc(sizeof(char) * (strlen(str_c1) + strlen(str_c2) + 2));
            str[0] = '\0';
            strcat(str, str_c1);
            str[strlen(str_c1)] = ' ';
            str[strlen(str_c1) + 1] = '\0';
            strcat(str, str_c2);
            send(sock, str, strlen(str), 0);
            free(str);
            free(str_c1);
            free(str_c2);

            mpz_clears(q, g, g1, data, c1, c2, NULL);
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