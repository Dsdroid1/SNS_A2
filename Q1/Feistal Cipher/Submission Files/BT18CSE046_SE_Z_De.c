#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

// Function declarations
int ReadKeys(char **k1, char **k2, int *key_size);
char *Decrypt(char *cipher_text, char *k1, char *k2, int key_size);

// Helper functions
void XORStrings(char *str1, char *str2, int key_size, char *result);
void RoundFunction(char *text, char *key, int key_size, char *result);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return 1;
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
    }

    return 0;
}

// Function definitions
int ReadKeys(char **k1, char **k2, int *key_size)
{
    FILE *fp;
    fp = fopen("keys.txt", "r");
    if (fp == NULL)
    {
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    sscanf(line, "%d", key_size);
    printf("Key Size:%d\n", *key_size);

    *k1 = (char *)malloc(sizeof(char) * (*key_size + 1));
    *k2 = (char *)malloc(sizeof(char) * (*key_size + 1));

    getline(&line, &len, fp);
    sscanf(line, "%s", *k1);
    getline(&line, &len, fp);
    sscanf(line, "%s", *k2);
    printf("k1:%s\n", *k1);
    printf("k2:%s\n", *k2);
    fclose(fp);
    return 0;
}

void XORStrings(char *str1, char *str2, int key_size, char *result)
{
    for (int i = 0; i < key_size; i++)
    {
        if (str1[i] == str2[i])
        {
            result[i] = '0';
        }
        else
        {
            result[i] = '1';
        }
    }
}

void RoundFunction(char *text, char *key, int key_size, char *result)
{
    // Assumed that text[] and key[] are of same size = key_size+1
    // Both text and key are binary representation strings
    // f = (text)XOR(46)XOR(key)
    int roll_number = 46;
    // Convert this to binary string within key_size(truncate)
    char c;
    char rn_component[key_size + 1];
    rn_component[key_size] = '\0';
    int writer = key_size - 1;
    while (roll_number > 0)
    {
        c = roll_number % 10 + '0';
        // printf(" %c ", c);
        roll_number /= 10;
        for (int i = 0; i <= 7 && writer >= 0; i++, writer--)
        {
            (c & (1 << i)) ? (rn_component[writer] = '1') : (rn_component[writer] = '0');
        }
    }
    while (writer >= 0)
    {
        rn_component[writer] = '0';
        writer--;
    }
    // printf("DEBUG %s\n", rn_component);
    // XOR text and rn_component
    XORStrings(text, rn_component, key_size, rn_component);
    XORStrings(rn_component, key, key_size, result);
}

char *Decrypt(char *cipher_text, char *k1, char *k2, int key_size)
{
    char r4[key_size + 1], l4[key_size + 1], interim[key_size + 1];
    r4[key_size] = '\0';
    l4[key_size] = '\0';
    interim[key_size] = '\0';
    strncpy(l4, cipher_text, key_size);
    strncpy(r4, cipher_text + key_size, key_size);

    char l5[key_size + 1], r5[key_size + 1];
    // ROUND 1
    l5[key_size] = '\0';
    r5[key_size] = '\0';
    strcpy(r5, l4);
    RoundFunction(l4, k2, key_size, interim);
    // XOR
    XORStrings(r4, interim, key_size, l5);

    // ROUND 2
    char l6[key_size + 1], r6[key_size + 1];
    l6[key_size] = '\0';
    r6[key_size] = '\0';
    strcpy(r6, l5);
    RoundFunction(l5, k1, key_size, interim);
    // XOR
    XORStrings(r5, interim, key_size, l6);
    // printf("DECRYPTION: %s%s\n", l6, r6);
    char *binary;
    binary = (char *)malloc(sizeof(char) * (2 * key_size + 1));
    binary[2 * key_size] = '\0';
    strcpy(binary, l6);
    strcpy(binary + key_size, r6);
    return binary;
}