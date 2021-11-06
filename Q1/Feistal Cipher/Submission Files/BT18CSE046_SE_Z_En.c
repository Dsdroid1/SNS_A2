#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

// Function declarations
int ReadKeys(char **k1, char **k2, int *key_size);
char *Encrypt(char *plain_text, char *k1, char *k2, int key_size);

// Helper functions
void XORStrings(char *str1, char *str2, int key_size, char *result);
void RoundFunction(char *text, char *key, int key_size, char *result);
void Pt2Binary(char *plain_text, int pt_size, char *binary, int key_size);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <message>\n", argv[0]);
    }
    else
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
            // printf("Enter message:");
            // gets(buf);
            strcpy(buf, argv[1]);
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

void Pt2Binary(char *plain_text, int pt_size, char *binary, int key_size)
{
    // For every character of plain text, we get 8 bits of binary data.
    if (8 * pt_size > key_size)
    {
        // Data cant be encrypted totally in single shot
        if (key_size % 8 == 0)
        {
            plain_text[(int)key_size / 8] = '\0';
            pt_size = (int)key_size / 8;
            printf("Plain text binary is larger than what can be encrpyted, truncating...\n");
            printf("New Truncated text:%s\n", plain_text);
        }
        else
        {
            // Some char to be included partially
            plain_text[(int)key_size / 8 + 1] = '\0';
            pt_size = (int)key_size / 8 + 1;
            printf("Plain text binary is larger than what can be encrpyted, truncating...\n");
            printf("New Truncated text(last char has partial bits):%s\n", plain_text);
        }
    }
    // else
    // {
    char c;
    int b_wtr = key_size - 1;
    for (int i = pt_size - 1; i >= 0; i--)
    {
        c = plain_text[i];
        // printf("%c\n", c);
        for (int j = 0; j <= 7 && b_wtr >= 0; j++)
        {
            (c & (1 << j)) ? (binary[b_wtr] = '1') : (binary[b_wtr] = '0');
            // printf("%c ", binary[b_wtr]);
            b_wtr--;
        }
    }
    while (b_wtr >= 0)
    {
        binary[b_wtr] = '0';
        b_wtr--;
    }
    // }
}

char *Encrypt(char *plain_text, char *k1, char *k2, int key_size)
{
    // Convert plaint text to binary representation
    int len = strlen(plain_text);
    char binary[2 * key_size + 1];
    binary[2 * key_size] = '\0';
    Pt2Binary(plain_text, len, binary, 2 * key_size);

    char left[key_size + 1], right[key_size + 1];
    strncpy(left, binary, key_size);
    left[key_size] = '\0';
    strncpy(right, binary + key_size, key_size);
    right[key_size] = '\0';
    printf("Plain Text:%s\n", binary);
    // printf("L :%s , R:%s\n", left, right);

    // ROUND 1
    char l2[key_size + 1], r2[key_size + 1], interim[key_size + 1];
    interim[key_size] = '\0';
    l2[key_size] = '\0';
    r2[key_size] = '\0';
    strcpy(l2, right);
    RoundFunction(right, k1, key_size, interim);
    // printf("OUTPUT of F:%s\n", interim);

    // XOR
    XORStrings(left, interim, key_size, r2);

    // ROUND 2
    char l3[key_size + 1], r3[key_size + 1];
    l3[key_size] = '\0';
    r3[key_size] = '\0';
    strcpy(l3, r2);
    RoundFunction(r2, k2, key_size, interim);
    // XOR
    XORStrings(l2, interim, key_size, r3);
    // printf("Cipher Text:%s%s\n", l3, r3);
    char *CT;
    CT = (char *)malloc(sizeof(char) * (2 * key_size + 1));
    CT[2 * key_size] = '\0';
    strcpy(CT, l3);
    strcpy(CT + key_size, r3);
    return CT;
}
