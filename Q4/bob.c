#include <stdio.h>
#include <winsock2.h>
#include "des_cfb.h"

int main()
{
    WSADATA wsaData = {0};
    SOCKET sock;
    SOCKADDR_IN SvrAddr = {0};
    SOCKADDR_IN CliAddr = {0};
    int port = 9002;

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
        // printf("Alice sent: %s\n", buf);
        // DEerypt this message using Bob-KDC key
        // Convert to integer Bits
        int *cipherBits = (int *)malloc(sizeof(int) * iRet);
        int block_size = 32; // Agreed parameter
        for (int i = 0; i < iRet; i++)
        {
            cipherBits[i] = buf[i] - '0';
        }
        // Decrypt these to get the plain text
        char *str = CFB_mode_DES_Decrypt(cipherBits, block_size, iRet, "Bob-KDC.txt");
        printf("Decrypted text by KDC(via Alice)[Decrypted with Bob-KDC key]:%s\n", str);
        // Get the sess key
        char conn1[10], sess_key[65];
        sscanf(str, "%s %s", conn1, sess_key);
        // Store the session key in local file
        FILE *fp;
        fp = fopen("Kab_Bob_copy.txt", "w");
        if (fp != NULL)
        {
            fprintf(fp, "%s", sess_key);
            fclose(fp);
            printf("Stored local copy of session key\n");
            // Generate a nonce for Alice
            srand(time(NULL));
            int nonce = rand() % 100003;
            printf("Nonce for Alice[To be sent encrypted with Kab]:%d\n", nonce);
            char nonce_to_alice[20];
            nonce_to_alice[0] = '\0';
            itoa(nonce, nonce_to_alice, 10);
            // Encrypt this using the session key
            int lenAlice;
            int *cipherAliceBits = CFB_mode_DES_Encrypt(nonce_to_alice, block_size, &lenAlice, "Kab_Bob_copy.txt");
            // Convert to char array
            char *msg_to_alice = (char *)malloc(sizeof(char) * (lenAlice + 1));
            msg_to_alice[lenAlice] = '\0';
            for (int i = 0; i < lenAlice; i++)
            {
                msg_to_alice[i] = cipherAliceBits[i] + '0';
            }
            // Send this to alice
            send(hClient, msg_to_alice, lenAlice, 0);
            free(cipherAliceBits);
            free(msg_to_alice);

            // Receive alice Rb-1
            buf[0] = '\0';
            iRet = recv(hClient, buf, 6000, 0);
            cipherAliceBits = (int *)malloc(sizeof(int) * iRet);
            for (int i = 0; i < iRet; i++)
            {
                cipherAliceBits[i] = buf[i] - '0';
            }
            char *nonce_from_alice = CFB_mode_DES_Decrypt(cipherAliceBits, block_size, iRet, "Kab_Bob_copy.txt");
            printf("Alice replied Rb-1 as[after decryption with session key]: %s\n", nonce_from_alice);
            int Rb_minus_1;
            sscanf(nonce_from_alice, "%d", &Rb_minus_1);
            if (Rb_minus_1 % 100003 == (nonce - 1) % 100003)
            {
                printf("Needham-Schroeder Protocol verification complete, sess key is valid!\n");
            }
            else
            {
                printf("Rb-1 does not match , not valid, terminating...\n");
            }
            free(nonce_from_alice);
            free(cipherAliceBits);
        }
        else
        {
            printf("Cant make a local copy of the key\n");
        }

        free(str);
        free(cipherBits);
    }
    else
    {
        printf("Error recv: %d\n", iRet);
    }
    closesocket(hClient);

    WSACleanup();
    return 0;
}