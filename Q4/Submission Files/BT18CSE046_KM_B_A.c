#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <winsock2.h>

// Pre-defined DES code
int IP[] =
    {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};

int E[] =
    {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1};

int P[] =
    {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25};

int FP[] =
    {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};

int S1[4][16] =
    {
        14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
        0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
        4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
        15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13};

int S2[4][16] =
    {
        15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
        3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
        0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
        13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9};

int S3[4][16] =
    {
        10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
        13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
        1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12};

int S4[4][16] =
    {
        7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
        13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
        10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
        3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14};

int S5[4][16] =
    {
        2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
        14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
        4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
        11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3};

int S6[4][16] =
    {
        12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
        10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
        9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
        4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13};

int S7[4][16] =
    {
        4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
        13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
        1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
        6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12};

int S8[4][16] =
    {
        13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
        1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
        7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
        2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11};

int PC1[] =
    {
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4};

int PC2[] =
    {
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32};

int SHIFTS[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// FILE *out;
int LEFT[17][32], RIGHT[17][32];
int IPtext[64];
int EXPtext[48];
int XORtext[48];
int X[8][6];
int X2[32];
int R[32];
int key56bit[56];
int key48bit[17][48];
int CIPHER[64];
int ENCRYPTED[64];

void expansion_function(int pos, int text)
{
    for (int i = 0; i < 48; i++)
    {
        if (E[i] == pos + 1)
        {
            EXPtext[i] = text;
        }
    }
}

int initialPermutation(int pos, int text)
{
    int i;
    for (i = 0; i < 64; i++)
    {
        if (IP[i] == pos + 1)
        {
            break;
        }
    }
    IPtext[i] = text;
}

int F1(int i)
{
    int r, c, b[6];

    for (int j = 0; j < 6; j++)
    {
        b[j] = X[i][j];
    }

    r = b[0] * 2 + b[5];
    c = 8 * b[1] + 4 * b[2] + 2 * b[3] + b[4];

    if (i == 0)
    {
        return S1[r][c];
    }
    else if (i == 1)
    {
        return S2[r][c];
    }
    else if (i == 2)
    {
        return S3[r][c];
    }
    else if (i == 3)
    {
        return S4[r][c];
    }
    else if (i == 4)
    {
        return S5[r][c];
    }
    else if (i == 5)
    {
        return S6[r][c];
    }
    else if (i == 6)
    {
        return S7[r][c];
    }
    else if (i == 7)
    {
        return S8[r][c];
    }
}

int XOR(int a, int b)
{
    return (a ^ b);
}

int ToBits(int value)
{
    int k, j, m;
    static int i;

    if (i % 32 == 0)
    {
        i = 0;
    }

    for (j = 3; j >= 0; j--)
    {
        m = 1 << j;
        k = value & m;
        if (k == 0)
        {
            X2[3 - j + i] = '0' - 48;
        }
        else
        {
            X2[3 - j + i] = '1' - 48;
        }
    }

    i = i + 4;
}

int SBox(int XORtext[])
{
    int k = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            X[i][j] = XORtext[k++];
        }
    }

    int value;
    for (int i = 0; i < 8; i++)
    {
        value = F1(i);
        ToBits(value);
    }
}

int PBox(int pos, int text)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if (P[i] == pos + 1)
        {
            break;
        }
    }
    R[i] = text;
}

void cipher(int Round, int mode)
{
    for (int i = 0; i < 32; i++)
    {
        expansion_function(i, RIGHT[Round - 1][i]);
    }

    for (int i = 0; i < 48; i++)
    {
        if (mode == 0)
        {
            XORtext[i] = XOR(EXPtext[i], key48bit[Round][i]);
        }
        else
        {
            XORtext[i] = XOR(EXPtext[i], key48bit[17 - Round][i]);
        }
    }

    SBox(XORtext);

    for (int i = 0; i < 32; i++)
    {
        PBox(i, X2[i]);
    }

    for (int i = 0; i < 32; i++)
    {
        RIGHT[Round][i] = XOR(LEFT[Round - 1][i], R[i]);
    }
}

void finalPermutation(int pos, int text)
{
    int i;
    for (i = 0; i < 64; i++)
    {
        if (FP[i] == pos + 1)
        {
            break;
        }
    }
    ENCRYPTED[i] = text;
}

int *Encryption(int plain[])
{
    for (int i = 0; i < 64; i++)
    {
        initialPermutation(i, plain[i]);
    }

    for (int i = 0; i < 32; i++)
    {
        LEFT[0][i] = IPtext[i];
    }

    for (int i = 32; i < 64; i++)
    {
        RIGHT[0][i - 32] = IPtext[i];
    }

    for (int k = 1; k < 17; k++)
    {
        cipher(k, 0);

        for (int i = 0; i < 32; i++)
            LEFT[k][i] = RIGHT[k - 1][i];
    }

    for (int i = 0; i < 64; i++)
    {
        if (i < 32)
        {
            CIPHER[i] = RIGHT[16][i];
        }
        else
        {
            CIPHER[i] = LEFT[16][i - 32];
        }
        finalPermutation(i, CIPHER[i]);
    }
    int *CT;
    CT = (int *)malloc(sizeof(int) * 64);

    for (int i = 0; i < 64; i++)
    {
        CT[i] = ENCRYPTED[i];
    }
    return CT;
}

int *Decryption(int plain[])
{
    for (int i = 0; i < 64; i++)
    {
        initialPermutation(i, plain[i]);
    }

    for (int i = 0; i < 32; i++)
    {
        LEFT[0][i] = IPtext[i];
    }

    for (int i = 32; i < 64; i++)
    {
        RIGHT[0][i - 32] = IPtext[i];
    }

    for (int k = 1; k < 17; k++)
    {
        cipher(k, 1);

        for (int i = 0; i < 32; i++)
        {
            LEFT[k][i] = RIGHT[k - 1][i];
        }
    }

    for (int i = 0; i < 64; i++)
    {
        if (i < 32)
        {
            CIPHER[i] = RIGHT[16][i];
        }
        else
        {
            CIPHER[i] = LEFT[16][i - 32];
        }
        finalPermutation(i, CIPHER[i]);
    }
    int *PT;
    PT = (int *)malloc(sizeof(int) * 64);
    for (int i = 0; i < 64; i++)
    {
        PT[i] = ENCRYPTED[i];
    }
    return PT;
}

void key56to48(int round, int pos, int text)
{
    int i;
    for (i = 0; i < 56; i++)
    {
        if (PC2[i] == pos + 1)
        {
            break;
        }
    }
    key48bit[round][i] = text;
}

int key64to56(int pos, int text)
{
    int i;
    for (i = 0; i < 56; i++)
    {
        if (PC1[i] == pos + 1)
        {
            break;
        }
    }
    key56bit[i] = text;
}

void key64to48(unsigned int key[])
{
    int k, backup[17][2];
    int CD[17][56];
    int C[17][28], D[17][28];

    for (int i = 0; i < 64; i++)
    {
        key64to56(i, key[i]);
    }

    for (int i = 0; i < 56; i++)
    {
        if (i < 28)
        {
            C[0][i] = key56bit[i];
        }
        else
        {
            D[0][i - 28] = key56bit[i];
        }
    }

    for (int x = 1; x < 17; x++)
    {
        int shift = SHIFTS[x - 1];

        for (int i = 0; i < shift; i++)
        {
            backup[x - 1][i] = C[x - 1][i];
        }

        for (int i = 0; i < (28 - shift); i++)
        {
            C[x][i] = C[x - 1][i + shift];
        }

        k = 0;
        for (int i = 28 - shift; i < 28; i++)
        {
            C[x][i] = backup[x - 1][k++];
        }

        for (int i = 0; i < shift; i++)
        {
            backup[x - 1][i] = D[x - 1][i];
        }

        for (int i = 0; i < (28 - shift); i++)
        {
            D[x][i] = D[x - 1][i + shift];
        }

        k = 0;
        for (int i = 28 - shift; i < 28; i++)
        {
            D[x][i] = backup[x - 1][k++];
        }
    }

    for (int j = 0; j < 17; j++)
    {
        for (int i = 0; i < 28; i++)
        {
            CD[j][i] = C[j][i];
        }

        for (int i = 28; i < 56; i++)
        {
            CD[j][i] = D[j][i - 28];
        }
    }

    for (int j = 1; j < 17; j++)
    {
        for (int i = 0; i < 56; i++)
        {
            key56to48(j, i, CD[j][i]);
        }
    }
}

void create16Keys(char *filename)
{
    FILE *pt = fopen(filename, "rb");
    unsigned int key[64];
    int i = 0, ch;

    while (!feof(pt))
    {
        ch = getc(pt);
        key[i++] = ch - '0';
    }
    key64to48(key);
    fclose(pt);
}

// Function declaration
int *GetBinaryData(char *str, int block_size, int *length_of_data);
char *GetTextualData(int *data, int len);
int *CFB_mode_DES_Encrypt(char *text, int block_size, int *length_of_CT, char *key_file);
char *CFB_mode_DES_Decrypt(int *cipherText, int block_size, int len, char *key_file);
int *XOR_blocks(int *block1, int *block2, int block_size);
void ShiftRegisterToLeft(int shiftRegister[64], int *block, int block_size);

// Driver program
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

// Function definitions
int *GetBinaryData(char *str, int block_size, int *length_of_data)
{
    // Convert str to an integer array of bits
    int *data;
    // Get the length of str
    int len = 8 * strlen(str); // 8 bits per character
    // The length of our data will be the closest multiple of 64 greater than len
    len = (len / block_size + (len % block_size != 0)) * block_size;
    data = (int *)malloc(sizeof(int) * len);
    // Pad the data with zeros
    for (int i = 0; i < len; i++)
    {
        data[i] = 0;
    }
    // Convert every character in str to binary representation
    char ch;
    for (int i = 0; i < strlen(str); i++)
    {
        ch = str[i];
        for (int j = 7; j >= 0; j--)
        {
            data[8 * i + 7 - j] = ((ch & (1 << j)) ? 1 : 0);
        }
    }
    *length_of_data = len;
    return data;
}

char *GetTextualData(int *data, int len)
{
    // Ignore last bits due to padding, if all 8 bits are 0
    char *str;
    str = (char *)malloc(sizeof(char) * (len / 8 + 1));
    int len_str = len / 8 + 1;
    str[len_str - 1] = '\0';
    int ch;
    for (int i = 0; i < len_str - 1; i++)
    {
        ch = 0;
        for (int j = 7; j >= 0; j--)
        {
            ch += (data[8 * i + 7 - j] << j);
        }
        str[i] = ch;
    }
    return str;
}

// FUnctions for CFB op mode
void ShiftRegisterToLeft(int shiftRegister[64], int *block, int block_size)
{
    if (block_size < 64)
    {
        // Shift the register
        for (int i = 0; i < 64 - block_size; i++)
        {
            shiftRegister[i] = shiftRegister[i + block_size];
        }
        for (int i = 0; i < block_size; i++)
        {
            shiftRegister[64 - block_size + i] = block[i];
        }
    }
}

int *XOR_blocks(int *block1, int *block2, int block_size)
{
    int *ans = (int *)malloc(block_size * sizeof(int));
    for (int i = 0; i < block_size; i++)
    {
        ans[i] = block1[i] ^ block2[i];
    }
    return ans;
}

int *CFB_mode_DES_Encrypt(char *text, int block_size, int *length_of_CT, char *key_file)
{
    int IV[64] = {0};
    int len = 0;
    int *data = GetBinaryData(text, block_size, &len);
    // printf("Bits before encryption:");
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%d", data[i]);
    // }
    create16Keys(key_file); // Get all DES round keys
    int iters = len / block_size;
    int *cipherText = NULL, *Temp = NULL, *temp_block = NULL;
    cipherText = (int *)malloc(sizeof(int) * len);
    for (int i = 0; i < iters; i++)
    {
        // Get the encrypted temp via IV
        Temp = Encryption(IV);
        // XOR the first block_size bits with Plain text
        temp_block = XOR_blocks(data + i * block_size, Temp, block_size);
        // temp_block is the CT for PT[i*block_size:i*block_size+block_size]
        for (int j = 0; j < block_size; j++)
        {
            cipherText[i * block_size + j] = temp_block[j];
        }
        // Shift register bits
        ShiftRegisterToLeft(IV, temp_block, block_size);
        free(Temp);
        free(temp_block);
    }
    free(data);
    *length_of_CT = len;
    // printf("\nBits after encryption:");
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%d", cipherText[i]);
    // }
    // printf("\n");
    return cipherText;
}

char *CFB_mode_DES_Decrypt(int *cipherText, int block_size, int len, char *key_file)
{
    int *plainText = NULL;
    plainText = (int *)malloc(sizeof(int) * len);
    int IV[64] = {0};
    create16Keys(key_file);
    // printf("\nBefore decryption:");
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%d", cipherText[i]);
    // }
    // printf("\n");
    int iters = len / block_size;
    int *Temp = NULL, *temp_block = NULL;
    for (int i = 0; i < iters; i++)
    {
        Temp = Encryption(IV);
        temp_block = XOR_blocks(Temp, cipherText + i * block_size, block_size);

        for (int j = 0; j < block_size; j++)
        {
            plainText[i * block_size + j] = temp_block[j];
        }

        ShiftRegisterToLeft(IV, cipherText + i * block_size, block_size);

        free(temp_block);
        free(Temp);
    }
    char *data;
    // printf("\nDecrypted bits:");
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%d", plainText[i]);
    // }
    // printf("\n");
    data = GetTextualData(plainText, len);
    // printf("\n%s\n", data);
    free(plainText);
    return data;
}
