#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "feistal_cipher.h"

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
        printf("Enter message:");
        gets(buf);
        char *k1, *k2;
        int key_size;
        if (ReadKeys(&k1, &k2, &key_size) < 0)
        {
            printf("Error reading keys");
        }
        // printf("keys: %s %s", k1, k2);
        char *data;
        // printf("BUFFER:%s", buf);
        data = Encrypt(buf, k1, k2, key_size);
        printf("Encrypted data: %s", data);
        send(sock, data, 2 * key_size + 1, 0);
        free(k1);
        free(k2);
        free(data);
    }
    else
    {
        printf("Error");
    }
    closesocket(sock);

    WSACleanup();
    return 0;
}