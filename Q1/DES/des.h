#ifndef DES_H
#define DES_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

void KeyGen();
void create16Keys();
// 64 bit block codes
int *Encryption(int plain[]);
int *Decryption(int plain[]);

#endif