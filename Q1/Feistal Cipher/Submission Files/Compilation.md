Keygen:
Compile: gcc -o keygen BT18CSE046_SE_Z_Kg.c
Execute: .\keygen <combined_key_size(k1+k2)>

Bob:
Compile: gcc -o bob BT18CSE046_SE_Z_De.c -lws2_32
Execute: .\bob

Alice:
Compile: gcc -o alice BT18CSE046_SE_Z_En.c -lws2_32
Execute: .\alice <message_to_encrypt>

\*\* Instructions:
Generate the required keys first.
Run bob.exe, which starts bob in listening mode
Run alice.exe, with the message to encrypt as command-line argument
Result is output on respective terminals
