#include <stdio.h>
#include <winsock2.h>
#include "des_cfb.h"

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
    char buf[6000];
    printf("Connection made by Alice\n");
    iRet = recv(hClient, buf, 6000, 0);
    if (iRet != SOCKET_ERROR)
    {
        buf[iRet] = '\0';
        printf("Alice sent: %s\n", buf);
        // Generate a session key for Alice and Bob
        KeyGen("Alice-Bob.txt");
        int Ra;
        char conn1[10], conn2[10];
        sscanf(buf, "%s %s %d", conn1, conn2, &Ra);
        // printf("%s %s %d\n", conn1, conn2, Ra);
        // Read the generated key for sending
        FILE *fp = fopen("Alice-Bob.txt", "r");
        if (fp != NULL)
        {
            char sess_key[65];
            sess_key[64] = '\0';
            fscanf(fp, "%s", sess_key);
            // printf("KEY: %s\n", sess_key);
            // Send the encrypted data back to Alice
            char message_to_bob[2000] = {"Alice \0"};
            strcat(message_to_bob, sess_key);
            // printf("BOM: %s\n", message_to_bob);
            // Encrypt this using Bob-KDC key
            int block_size = 32, len;
            int *cipherBobBits = CFB_mode_DES_Encrypt(message_to_bob, block_size, &len, "Bob-KDC.txt");
            // Make message for Alice
            buf[0] = '\0';
            itoa(Ra, buf, 10);
            strcat(buf, " Bob ");
            strcat(buf, sess_key);
            strcat(buf, " ");
            int len_str = strlen(buf);
            for (int i = 0; i < len; i++)
            {
                buf[len_str + i] = cipherBobBits[i] + '0';
            }
            // printf("\n%d\n", len);
            buf[len_str + len] = '\0';
            printf("Alice message(before encryption): %s\n", buf);
            // printf("\n%d", strlen(buf));
            // Encrpyt this whole message with Alice Key
            int final_len_of_msg;
            int *cipherAliceBits = CFB_mode_DES_Encrypt(buf, block_size, &final_len_of_msg, "Alice-KDC.txt");
            // printf("%d\n", final_len_of_msg);
            buf[0] = '\0';
            for (int i = 0; i < final_len_of_msg; i++)
            {
                buf[i] = cipherAliceBits[i] + '0';
            }
            buf[final_len_of_msg] = '\0';
            send(hClient, buf, final_len_of_msg, 0);
            free(cipherBobBits);
            free(cipherAliceBits);
            printf("Sent the data to Alice(with encryption)\n");
        }
        else
        {
            printf("Keygen failed!\n");
        }
    }
    else
    {
        printf("Error recv: %d\n", iRet);
    }
    closesocket(hClient);

    WSACleanup();
    return 0;
}