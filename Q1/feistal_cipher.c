#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "feistal_cipher.h"

void KeyGen(int bits)
{
    if (bits % 2 != 0)
    {
        bits = 1024;
    }
    if (bits > 1024)
    {
        bits = 1024;
        // Practical limits on key sizes
    }
    char k1[bits / 2 + 1], k2[bits / 2 + 1];
    // k1 and k2 are the 2 round keys for our feistal cipher
    int bit1, bit2;
    for (int i = 0; i < bits / 2; i++)
    {
        // Generate random bits for the ith position in our keys
        bit1 = rand() % 2;
        bit2 = rand() % 2;
        k1[i] = bit1 + '0';
        k2[i] = bit2 + '0';
    }
    k1[bits / 2] = '\0';
    k2[bits / 2] = '\0';
    printf("%s\n", k1);
    printf("%s\n", k2);
    FILE *fp;
    fp = fopen("keys.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", bits / 2);
        fprintf(fp, "%s\n", k1);
        fprintf(fp, "%s\n", k2);
    }
    fclose(fp);
}

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
        for (int j = 0; j <= 7; j++)
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

// void main()
// {
//     // KeyGen(10);
//     // char pt[3] = "ab";
//     // char binary[17];
//     // binary[16] = '\0';
//     // Pt2Binary(pt, 2, binary, 16);
//     // printf("%s\n", binary);
//     ///// TEST RUn
//     // char k1[9] = "10101010", k2[9] = "00101010";
//     // k1[8] = '\0';
//     // k2[8] = '\0';
//     // char pt[2] = "a";
//     // int key_size = 8;
//     // char *CT = Encrypt(pt, k1, k2, key_size);
//     // // printf("%s", CT);
//     // char *PT = Decrypt(CT, k1, k2, key_size);
//     // free(CT);
//     // free(PT);
//     // KeyGen(10);
//     char *k1, *k2;
//     int key_size;
//     ReadKeys(k1, k2, &key_size);
// }