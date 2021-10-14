CC = gcc
CFLAGS = "-lws2_32"

keygen: keygen.o feistal_cipher.o
	$(CC) -o $@ $^ $(CFLAGS)

alice: Alice.o feistal_cipher.o
	$(CC) -o $@ $^ $(CFLAGS)

bob: Bob.o feistal_cipher.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

