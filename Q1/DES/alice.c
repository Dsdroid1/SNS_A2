#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "des_cfb.h"

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
        // Initialize DES keys doen in wrapper function
        printf("Enter the message to send to Bob:");
        // scanf("%4999s", buf);
        gets(buf);
        int r = 40, cipherLen = 0;
        int *cipherBits = CFB_mode_DES_Encrypt(buf, 40, &cipherLen, "key.txt");
        // Convert this to string
        char *str;
        str = (char *)malloc(sizeof(char) * (cipherLen + 1));
        str[cipherLen] = '\0';
        for (int i = 0; i < cipherLen; i++)
        {
            str[i] = cipherBits[i] + '0';
        }
        // Send this char array to Bob
        send(sock, str, cipherLen, 0);
        free(cipherBits);
        free(str);
    }
    else
    {
        printf("Error");
    }
    closesocket(sock);

    WSACleanup();
    return 0;
}