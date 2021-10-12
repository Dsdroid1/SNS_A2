#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "elgamal.h"

int main()
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
        gmp_printf("Enter message(Prime q=%Zd)[Message number between 1 and q-1]:", q);
        gets(buf);
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
    return 0;
}