#include <stdio.h>
#include "fiat_shamir.h"

void main()
{
    int bits;
    printf("Enter the total key size(from which prime p,q are to be choosen):");
    scanf("%d", &bits);
    KeyGen(bits);
    printf("Done!\n");
}