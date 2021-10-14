#include <stdio.h>
#include "des.h"

int main()
{
    printf("Generating a random 64 bit key[bits will be dropped by functions]");
    KeyGen();
    return 0;
}