#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void KeyGen();

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }
    else
    {
        printf("Generating a random 64 bit key[bits will be dropped by functions]");
        KeyGen();
    }
    return 0;
}

// Function definitions
void KeyGen()
{
    // Generate a random 64 bit key, which will be reduced to 56 bit in DES
    char key[65];
    srand(time(0));
    for (int i = 0; i < 64; i++)
    {
        key[i] = rand() % 2 + '0';
    }
    key[64] = '\0';
    FILE *fp;
    fp = fopen("key.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%s", key);
    }
    fclose(fp);
}