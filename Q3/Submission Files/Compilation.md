gcc -o keygen BT18CSE046_EA_B_Kg.c -lgmp
gcc -o alice BT18CSE046_EA_B_A.c -lgmp -lws2_32
gcc -o bob BT18CSE046_EA_B_B.c -lgmp -lws2_32

Run keygen, then bob and then alice
