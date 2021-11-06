#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <gmp.h>

// Function declarations
void Decrypt(mpz_t c1, mpz_t c2, mpz_t q, mpz_t x, mpz_t PT);
int ReadSK(mpz_t q, mpz_t x);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return -1;
    }
    else
    {
        WSADATA wsaData = {0};
        SOCKET sock;
        SOCKADDR_IN SvrAddr = {0};
        SOCKADDR_IN CliAddr = {0};
        int port = 9001;

        WSAStartup(MAKEWORD(2, 2), &wsaData);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            printf("Error in making socket!");
            return -1;
        }
        printf("Socket created\n");

        SvrAddr.sin_family = AF_INET;
        SvrAddr.sin_port = htons(port);
        SvrAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (SOCKADDR *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
        {
            printf("Error in binding");
            closesocket(sock);
            return -1;
        }

        listen(sock, 3);

        int iSize = sizeof(CliAddr);
        SOCKET hClient = accept(sock, (SOCKADDR *)&CliAddr, &iSize);
        if (hClient == INVALID_SOCKET)
        {
            printf("Error, client conn");
            return -1;
        }
        int iRet;
        char buf[5000];
        printf("Connection made\n");
        iRet = recv(hClient, buf, 5000, 0);
        if (iRet != SOCKET_ERROR)
        {
            buf[iRet] = '\0';
            printf("Alice sent(encrpyted): %s\n", buf);
            mpz_t q, x, c1, c2, pt;
            mpz_inits(q, x, c1, c2, pt, NULL);
            char *str_c1, *str_c2;
            str_c1 = (char *)malloc(sizeof(char) * strlen(buf));
            str_c2 = (char *)malloc(sizeof(char) * strlen(buf));
            sscanf(buf, "%s %s", str_c1, str_c2);
            mpz_set_str(c1, str_c1, 10);
            mpz_set_str(c2, str_c2, 10);

            ReadSK(q, x);
            Decrypt(c1, c2, q, x, pt);
            gmp_printf("Decrypted:%Zd\n", pt);
            free(str_c1);
            free(str_c2);

            mpz_clears(q, x, c1, c2, pt, NULL);
        }
        else
        {
            printf("Error recv: %d\n", iRet);
        }
        closesocket(hClient);

        WSACleanup();
    }

    return 0;
}

// Function definitions
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