/*
** server.c -- a stream socket server demo
** Server side program that is able to wait for a connection and answer with a
** Hello World string to a client program
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

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(myPort);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

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
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        if (!fork())
        { // this is the child process
            signal(SIGCHLD, SIG_IGN);
            sleep(5);
            while (1)
            {
                memset(buf, 0, MAXDATASIZE);
                if ((retVal = read(new_fd, buf, MAXDATASIZE)) == -1)
                {
                    perror("Receiving");
                    close(new_fd);
                    exit(-1);
                }
                if ((retVal = write(new_fd, buf, retVal)) == -1)
                {
                    perror("Sending");
                    close(new_fd);
                    exit(-1);
                }
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