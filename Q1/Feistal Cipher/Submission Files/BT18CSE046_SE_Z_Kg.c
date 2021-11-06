#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// Functions declarations
void KeyGen(int bits);

// Driver program
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <combined_bit_size(k1+k2)>\n", argv[0]);
    }
    else
    {
        int key_size;
        key_size = atoi(argv[1]);
        KeyGen(key_size);
        printf("Done!\n");
    }

    return 0;
}

// Function definitions
void KeyGen(int bits)
{
    if (bits % 2 != 0)
    {
        bits = 1024;
    }
    if (bits > 1024)
    {
        bits = 1024;
        // Practical limits on key sizes
    }
    char k1[bits / 2 + 1], k2[bits / 2 + 1];
    // k1 and k2 are the 2 round keys for our feistal cipher
    int bit1, bit2;
    for (int i = 0; i < bits / 2; i++)
    {
        // Generate random bits for the ith position in our keys
        bit1 = rand() % 2;
        bit2 = rand() % 2;
        k1[i] = bit1 + '0';
        k2[i] = bit2 + '0';
    }
    k1[bits / 2] = '\0';
    k2[bits / 2] = '\0';
    printf("%s\n", k1);
    printf("%s\n", k2);
    FILE *fp;
    fp = fopen("keys.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", bits / 2);
        fprintf(fp, "%s\n", k1);
        fprintf(fp, "%s\n", k2);
    }
    fclose(fp);
}
