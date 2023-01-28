/*
** server.c -- a stream socket server demo
** Server side program that is able to wait for a connection and answer with a
** what the client sent back
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "api.h"

#define MYPORT 3490     // the port users will be connecting to
#define MAXDATASIZE 200 // max number of bytes we can get at once

#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

int main(int argc, char **argv)
{
    int sockfd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    int myPort;
    int retVal;
    char buf[MAXDATASIZE];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Port Number>\n", argv[0]);
        exit(-1);
    }
    myPort = atoi(argv[1]);

    // Aloca o socket (primeiro argumento fala que vai ser um socket de internet
    // o segundo que é TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(-2);
    }
    /*
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(-3);
        }
    */

    my_addr.sin_family = AF_INET;         // especifica que é um endereço da internet
    my_addr.sin_port = htons(myPort);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // aceita requisições de qualquer lugar
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    // Associa esse socket a esse endereço
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // começa a ouvir conexões nesse socket
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    while (1)
    { // main accept() loop

        printf("Accepting ....\n");
        retVal = 0;
        sin_size = sizeof(struct sockaddr_in);
        // aceita conexões, o segundo argumento é o endereço de quem fez a
        // requisição, e o terceiro é o tamanho
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        // retorna um processo filho para cuidar da requisição enquanto o processo
        // pai continua cuidando de receber conexões
        if (!fork())
        { // this is the child process
            // ignora o sinal que é mandado quando o filho morre :(
            signal(SIGCHLD, SIG_IGN);
            sleep(5);
            while (1)
            {
                // aqui a mágica começa.
                // Agora esse processo é o que vai cuidar da requisição
                memset(buf, 0, MAXDATASIZE);
                if ((retVal = read(new_fd, buf, MAXDATASIZE - 1)) == -1)
                {
                    perror("Receiving");
                    close(new_fd);
                    exit(-1);
                }

                char *res = handle_request(buf, &their_addr);

                // escreve no file descriptor (nosso caso a conexão TCP)
                if ((retVal = write(new_fd, res, strlen(res))) == -1)
                {
                    perror("Sending");
                    close(new_fd);
                    exit(-1);
                }
                // se o client pediu pra parar a gente para
                if (!strcmp(buf, "quit"))
                {
                    printf("Connection closed by foreign host\n");
                    sleep(1);
                    close(new_fd);
                    exit(-1);
                }
            }
        }
        close(new_fd); // parent doesn't need this
    }

    return 0;
}