#include <stdio.h>
#include <winsock2.h>
#include "feistal_cipher.h"

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
        char *k1, *k2;
        int key_size;
        ReadKeys(&k1, &k2, &key_size);
        char *plain_text;
        plain_text = Decrypt(buf, k1, k2, key_size);
        printf("Decrpyted data:%s\n", plain_text);
        free(k1);
        free(k2);
        free(plain_text);
    }
    else
    {
        printf("Error recv: %d\n", iRet);
    }
    closesocket(hClient);

    WSACleanup();
    return 0;
}