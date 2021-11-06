gcc -o keygen BT18CSE046_KM_B_Kg.c
gcc -o kdc BT18CSE046_KM_B_Kdc.c -lws2_32
gcc -o alice BT18CSE046_KM_B_A.c -lws2_32
gcc -o bob BT18CSE046_KM_B_B.c -lws2_32

run keygen
run bob, kdc
run alice
