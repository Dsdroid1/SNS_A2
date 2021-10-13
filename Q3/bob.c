#include <stdio.h>
#include <winsock2.h>
#include <gmp.h>
#include "fiat_shamir.h"

int main()
{
    WSADATA wsaData = {0};
    SOCKET sock;
    SOCKADDR_IN SvrAddr = {0};
    SOCKADDR_IN CliAddr = {0};
    int port = 9001;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("Error in making socket!");
        return -1;
    }
    printf("Socket created\n");

    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (SOCKADDR *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
    {
        printf("Error in binding");
        closesocket(sock);
        return -1;
    }

    listen(sock, 3);

    int iSize = sizeof(CliAddr);
    SOCKET hClient = accept(sock, (SOCKADDR *)&CliAddr, &iSize);
    if (hClient == INVALID_SOCKET)
    {
        printf("Error, client conn");
        return -1;
    }
    mpz_t n, V, x, y;
    mpz_inits(n, V, x, y, NULL);
    if (ReadPK(n, V) < 0)
    {
        printf("Keys not available!\n");
        return -1;
    }
    int iRet;
    char buf[5000];
    printf("Connection made\n");
    int num_rounds = 3, curr_round = 1;
    while (curr_round <= num_rounds)
    {
        printf("Round %d\n", curr_round);
        // Receive witness from Alice
        iRet = recv(hClient, buf, 5000, 0);
        if (iRet != SOCKET_ERROR)
        {
            buf[iRet] = '\0';
            printf("Alice sent witness x: %s\n", buf);
            mpz_set_str(x, buf, 10);
            // Send back the challenge
            int c = choose_challenge();
            printf("Bob has choosen challenge: %d\n", c);
            char challenge[2];
            challenge[1] = '\0';
            challenge[0] = c + '0';

            send(hClient, challenge, 1, 0);
            printf("Successfully sent challenge c to Alice\n");

            // Get the response from Alice
            iRet = recv(hClient, buf, 5000, 0);
            if (iRet != SOCKET_ERROR)
            {
                buf[iRet] = '\0';
                printf("Alice sent response:%s\n", buf);
                printf("Now verifying response...\n");
                mpz_set_str(y, buf, 10);
                // gmp_printf("%Zd\n", y);
                int valid = verify_response(n, x, V, c, y);
                if (valid == 1)
                {
                    printf("Response verified, and is correct!\n");
                }
                else
                {
                    printf("Response^2 does match x*V^c, hence invalid");
                    return -1;
                }
            }
            else
            {
                printf("Error receiving response from Alice\n");
            }
        }
        else
        {
            printf("Error recv: %d\n", iRet);
        }
        curr_round++;
    }

    closesocket(hClient);
    mpz_clears(n, V, x, y, NULL);

    WSACleanup();
    return 0;
}