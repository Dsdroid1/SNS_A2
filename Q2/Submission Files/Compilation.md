gcc -o bob .\BT18CSE046_AC_B_De.c -lgmp -lws2_32
gcc -o alice .\BT18CSE046_AC_B_En.c -lgmp -lws2_32
gcc -o keygen BT18CSE046_AC_B_Kg.c -lgmp

./bob.exe
./alice.exe <number_to_send>
