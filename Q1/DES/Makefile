CC = gcc
CFLAGS = -lws2_32

keygen: keygen.o des.o
	$(CC) -o $@ $^ $(CFLAGS)

alice: alice.o des_cfb.o des.o
	$(CC) -o $@ $^ $(CFLAGS)

bob: bob.o des_cfb.o des.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ 

