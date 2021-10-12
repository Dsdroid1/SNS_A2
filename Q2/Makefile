CC = gcc
CFLAGS = -lws2_32 -lgmp

keygen: keygen.o elgamal.o
	$(CC) -o $@ $^ $(CFLAGS)

alice: alice.o elgamal.o
	$(CC) -o $@ $^ $(CFLAGS)

bob: bob.o elgamal.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ 

