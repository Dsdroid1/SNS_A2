#include <stdio.h>
#include <string.h>
#include "des.h"

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

int *CFB_mode_DES_Encrypt(char *text, int block_size, int *length_of_CT)
{
    int IV[64] = {0};
    int len = 0;
    int *data = GetBinaryData(text, block_size, &len);
    create16Keys("key.txt"); // Get all DES round keys
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
    return cipherText;
}

char *CFB_mode_DES_Decrypt(int *cipherText, int block_size, int len)
{
    int *plainText = NULL;
    plainText = (int *)malloc(sizeof(int) * len);
    int IV[64] = {0};
    create16Keys("key.txt");
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
    data = GetTextualData(plainText, len);
    // printf("\n%s\n", data);
    free(plainText);
    return data;
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
    //////// DES TEST
    // char str[100] = "Hello\0";
    // int len = 0;
    // int *data = GetBinaryData(str, 64, &len);
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%d", data[i]);
    // }
    // char *text;
    // text = GetTextualData(data, len);
    // int len_str = strlen(text);
    // printf("\n");
    // for (int i = 0; i < len_str; i++)
    // {
    //     printf("%c", text[i]);
    // }
    // free(text);
    // free(data);
    char *str = "Hello this is some long text, which should be encryted\0";
    int len;
    int *cipherBits = CFB_mode_DES_Encrypt(str, 40, &len);
    printf("Encrypted...\n");
    char *plainText = CFB_mode_DES_Decrypt(cipherBits, 40, len);
    printf("Decrypted:%s\n", plainText);
    return 0;
}