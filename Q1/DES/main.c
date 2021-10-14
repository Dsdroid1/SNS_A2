#include <stdio.h>
#include <string.h>
#include "des_cfb.h"

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