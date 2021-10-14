#include <stdio.h>
#include <string.h>
#include "des_cfb.h"

int main()
{
    // KeyGen();
    char *str = "Hello this is some long text, which should be encryted\0";
    int len, r = 40;
    int *cipherBits = CFB_mode_DES_Encrypt(str, r, &len);
    printf("Encrypted...\n");
    char *plainText = CFB_mode_DES_Decrypt(cipherBits, r, len);
    printf("Decrypted:%s\n", plainText);
    free(plainText);
    free(cipherBits);
    return 0;
}