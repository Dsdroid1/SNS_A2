CC = gcc
CFLAGS = -lws2_32 -lgmp

keygen: keygen.o fiat_shamir.o
	$(CC) -o $@ $^ $(CFLAGS)

alice: alice.o fiat_shamir.o
	$(CC) -o $@ $^ $(CFLAGS)

bob: bob.o fiat_shamir.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ 