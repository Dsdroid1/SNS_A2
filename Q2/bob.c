#include <stdio.h>
#include <winsock2.h>
#include "elgamal.h"

int main()
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
        // char *k1, *k2;
        // int key_size;
        // ReadKeys(&k1, &k2, &key_size);
        // char *plain_text;
        // plain_text = Decrypt(buf, k1, k2, key_size);
        // printf("Decrpyted data:%s\n", plain_text);
        // free(k1);
        // free(k2);
        // free(plain_text);
    }
    else
    {
        printf("Error recv: %d\n", iRet);
    }
    closesocket(hClient);

    WSACleanup();
    return 0;
}