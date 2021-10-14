#ifndef DES_CFB_H
#define DES_CFB_H

#include <stdio.h>
#include <string.h>
#include "des.h"

int *GetBinaryData(char *str, int block_size, int *length_of_data);
char *GetTextualData(int *data, int len);
int *CFB_mode_DES_Encrypt(char *text, int block_size, int *length_of_CT);
char *CFB_mode_DES_Decrypt(int *cipherText, int block_size, int len);
// Can make IV from a file, not yet done

#endif