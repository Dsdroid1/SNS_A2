#include <stdio.h>
#include "feistal_cipher.h"

int main()
{
    int key_size;
    printf("Enter the total key size(k1+k2), should be even:");
    scanf("%d", &key_size);
    KeyGen(key_size);
    printf("Done!\n");
    return 0;
}