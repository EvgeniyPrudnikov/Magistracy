#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{

    if (argc > 2) std::cerr << "Wrong number of arguments!" << std::endl;

    const char *SOCK_PATH = argv[1];
    char response;
    bool guess = false;

    int sock, t, len;
    struct sockaddr_un remote;
    int number = rand() % 1000000000;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    std::cerr<<"Trying to connect..."<<std::endl;

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(sock, (struct sockaddr *) &remote, len) == -1)
    {
        perror("connect");
        exit(1);
    }

    std::cerr << "Connected." << std::endl;

    while (!guess)
    {

        if (send(sock, &number, sizeof(int), 0) == -1)
        {
            perror("send");
            exit(1);
        }

        if ((t = (int) recv(sock, &response, 1, 0)) > 0)
        {
            if (response == '>')
            {
                number /= 2;

            } else if (response == '<')
            {
                number *= 2;

            } else
            {
                guess = true;
            }

        } else
        {
            if (t < 0) perror("recv");
            else std::cerr<<"Server closed connection"<<std::endl;
            exit(1);
        }
    }

    close(sock);

    return 0;
}