#include <stdio.h>
#include <string.h>
#include "des.h"

int *GetBinaryData(char *str, int *length_of_data)
{
    // Convert str to an integer array of bits
    int *data;
    // Get the length of str
    int len = 8 * strlen(str); // 8 bits per character
    // The length of our data will be the closest multiple of 64 greater than len
    len = (len / 64 + (len % 64 != 0)) * 64;
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

int main()
{
    // KeyGen();
    // create16Keys(); // Key in global vars
    // int PT[64] = {0};
    // printf("Plain Text:");
    // for (int i = 0; i < 64; i++)
    // {
    //     printf("%d", PT[i]);
    // }
    // int *CT;
    // CT = Encryption(PT);
    // printf("\nCipher Text:");
    // for (int i = 0; i < 64; i++)
    // {
    //     printf("%d", CT[i]);
    // }
    // printf("\nDecrypted text:");
    // int *decrypted = Decryption(CT);
    // for (int i = 0; i < 64; i++)
    // {
    //     printf("%d", decrypted[i]);
    // }
    // printf("\n");
    // free(CT);
    // free(decrypted);
    char str[100] = "Hello\0";
    int len = 0;
    int *data = GetBinaryData(str, &len);
    for (int i = 0; i < len; i++)
    {
        printf("%d", data[i]);
    }
    char *text;
    text = GetTextualData(data, len);
    int len_str = strlen(text);
    printf("\n");
    for (int i = 0; i < len_str; i++)
    {
        printf("%c", text[i]);
    }
    free(text);
    free(data);
    return 0;
}