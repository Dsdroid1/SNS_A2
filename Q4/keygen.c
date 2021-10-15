#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "des.h"

int main()
{
    printf("Generating a random 64 bit key[bits will be dropped by functions]");
    srand(time(NULL));
    KeyGen("Alice-KDC.txt");
    KeyGen("Bob-KDC.txt");
    return 0;
}