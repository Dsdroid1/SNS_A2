#include <stdio.h>
#include "elgamal.h"

void main()
{
    int bits;
    printf("Enter the total key size(from which prime q is to be choosen):");
    scanf("%d", &bits);
    KeyGen(bits);
    printf("Done!\n");
}