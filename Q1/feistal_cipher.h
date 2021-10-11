#ifndef FEISTAL_CIPHER
#define FEISTAL_CIPHER

void KeyGen(int bits);
char *Encrypt(char *plain_text, char *k1, char *k2, int key_size);
char *Decrypt(char *cipher_text, char *k1, char *k2, int key_size);
int ReadKeys(char **k1, char **k2, int *key_size);

#endif