#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>
#include <ctime>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Wrong number of arguments!\nUsage: ./guess-client UNIX_SOCKET_PATH\n";
        return 1;
    }

    const char *SOCK_PATH = argv[1];
    char response;
    bool guess = false;

    int sock, t, len;
    struct sockaddr_un remote;
    //srand(time(NULL));
    int start = 0;
    int end = 1000000000;
    int number = 500000000;// rand()%1000000001;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return 2;
    }

    std::cerr << "Trying to connect..." << std::endl;

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(sock, (struct sockaddr *) &remote, len) == -1)
    {
        perror("connect");
        return 2;
    }

    std::cerr << "Connected." << std::endl;

    while (!guess)
    {
        int num_for_send = __builtin_bswap32(number);
        if (send(sock, &num_for_send, sizeof(int), 0) == -1)
        {
            perror("send");
            return 2;
        }

        if ((t = (int) recv(sock, &response, sizeof(char), 0)) > 0)
        {
            if (response == '>')
            {
                start = number;
                number += (end - start) / 2;

            } else if (response == '<')
            {
                end = number;
                number -= (end - start) / 2;

            } else
            {
                guess = true;
            }

        } else
        {
            if (t < 0) perror("recv");
            else std::cerr << "Server closed connection" << std::endl;
            return 2;
        }
    }
    
    std::cout<<number<<std::endl;

    std::cerr<<"Guess!\nClosing connection!"<<std::endl;
    close(sock);

    return 0;
}