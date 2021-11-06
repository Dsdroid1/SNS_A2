#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void KeyGen(char *filename);
// Driver program
int main()
{
    printf("Generating Alice-KDC and Bob-KDC keys...");
    srand(time(NULL));
    KeyGen("Alice-KDC.txt");
    KeyGen("Bob-KDC.txt");
    printf("\nDone!");
    return 0;
}

// Function definitions
void KeyGen(char *filename)
{
    // Generate a random 64 bit key, which will be reduced to 56 bit in DES
    char key[65];
    // srand(time(0));
    for (int i = 0; i < 64; i++)
    {
        key[i] = rand() % 2 + '0';
    }
    key[64] = '\0';
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp != NULL)
    {
        fprintf(fp, "%s", key);
    }
    fclose(fp);
}