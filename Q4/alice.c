#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "des_cfb.h"

int main()
{
    WSADATA wsaData = {0};
    SOCKET kdc, bob;
    SOCKADDR_IN KdcAddr = {0};
    SOCKADDR_IN BobAddr = {0};
    int kdc_port = 9001, bob_port = 9002;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    kdc = socket(AF_INET, SOCK_STREAM, 0);
    bob = socket(AF_INET, SOCK_STREAM, 0);

    KdcAddr.sin_family = AF_INET;
    KdcAddr.sin_port = htons(kdc_port);
    KdcAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    BobAddr.sin_family = AF_INET;
    BobAddr.sin_port = htons(bob_port);
    BobAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(kdc, (SOCKADDR *)&KdcAddr, sizeof(KdcAddr)) != SOCKET_ERROR)
    {
        char buf[6000] = {"Alice Bob \0"};
        int len = strlen(buf);
        printf("Conn to KDC established!\n");
        srand(time(NULL)); // RNG for nonce
        int nonce = rand() % 100003;
        printf("Ra(Alice nonce):%d\n", nonce);
        itoa(nonce, buf + len, 10);
        send(kdc, buf, strlen(buf), 0);
        buf[0] = '\0';
        // Receive response from KDC
        int iRet = recv(kdc, buf, 6000, 0);
        if (iRet != SOCKET_ERROR)
        {
            // Received the key scucessfully
            int *cipherBits;
            cipherBits = (int *)malloc(sizeof(int) * iRet);
            for (int i = 0; i < iRet; i++)
            {
                cipherBits[i] = buf[i] - '0';
            }
            int block_size = 32;
            char *message = CFB_mode_DES_Decrypt(cipherBits, block_size, iRet, "Alice-KDC.txt");
            printf("Decrypted KDC message with Alice-KDC key:%s\n", message);
            // Scan the nonce and verify
            int response_nonce;
            // Extract the Sess key and Bob's part of the message
            char conn2[10], sess_key[65], message_to_bob[6000];
            sscanf(message, "%d %s %s %s", &response_nonce, conn2, sess_key, message_to_bob);
            if (response_nonce == nonce)
            {
                printf("Valid nonce from KDC\n");
                // Make a local copy of the sess_key
                FILE *fp;
                fp = fopen("Kab_Alice_copy.txt", "w");
                if (fp != NULL)
                {
                    fprintf(fp, "%s", sess_key);
                    fclose(fp);
                    printf("Stored the session key locally\n");
                    if (connect(bob, (SOCKADDR *)&BobAddr, sizeof(BobAddr)) != SOCKET_ERROR)
                    {
                        printf("Conn established with Bob\n");
                        // printf("\n Len msg to Bob:%d", strlen(message_to_bob));
                        send(bob, message_to_bob, strlen(message_to_bob), 0);
                        buf[0] = '\0';
                        // Receive Rb from Bob
                        iRet = recv(bob, buf, 6000, 0);
                        if (iRet != SOCKET_ERROR)
                        {
                            // Decrypt message from Bob using session key
                            int Rb;
                            int *cipherBobBits = (int *)malloc(sizeof(int) * iRet);
                            for (int i = 0; i < iRet; i++)
                            {
                                cipherBobBits[i] = buf[i] - '0';
                            }
                            char *nonce_from_bob = CFB_mode_DES_Decrypt(cipherBobBits, block_size, iRet, "Kab_Alice_copy.txt");
                            printf("Nonce from Bob[Decrypted using session key]:%s\n", nonce_from_bob);
                            sscanf(nonce_from_bob, "%d", &Rb);
                            // Calculate Rb-1, encrypt and send back
                            Rb = Rb - 1 % 100003;
                            printf("Rb-1[will be sent encrypted using session key(Kab)]: %d\n", Rb);
                            message_to_bob[0] = '\0';
                            free(cipherBobBits);
                            itoa(Rb, message_to_bob, 10);
                            int lenBob;
                            cipherBobBits = CFB_mode_DES_Encrypt(message_to_bob, block_size, &lenBob, "Kab_Alice_copy.txt");
                            // Convert to char for sending
                            message_to_bob[lenBob] = '\0';
                            for (int i = 0; i < lenBob; i++)
                            {
                                message_to_bob[i] = cipherBobBits[i] + '0';
                            }
                            send(bob, message_to_bob, lenBob, 0);
                            free(cipherBobBits);
                            free(nonce_from_bob);
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        printf("Cannot connect to Bob\n");
                    }
                }
                else
                {
                    printf("Cant make a local copy of the key\n");
                }
                // printf("%s\n", conn2);
                // printf("%s\n", sess_key);
                // Connect to Bob and give him the message from KDC
            }
            else
            {
                printf("Invalid nonce, terminating...\n");
            }
            free(message);
            free(cipherBits);
        }
        else
        {
            printf("Error in receiving data from KDC\n");
        }
    }
    else
    {
        printf("Error in connecting to KDC\n");
    }
    closesocket(kdc);

    WSACleanup();
    return 0;
}