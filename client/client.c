/*
** client.c -- a stream socket client demo
** Client side program that is able to establish a connection with a server
** receive a message and print the received string.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "user_input.h"
#include "queue.h"
#include "../helpers/helper.h"

#define PORT 3490 // the port client will be connecting to

#define MAXDATASIZE 5000 // max number of bytes we can get at once

enum t_state
{
    IDLE,
    CONNECTED,
    HHHH
} state;

int sockfd, numbytes;

void sigControlC()
{
    printf("Control C pressed\n");
    close(sockfd);
    exit(0);
}

void sigHandler()
{
    fprintf(stderr, "*** Pegou um sinal ***\n");
}

void showMessages(Queue *queue)
{
    if (queue_size(queue) == 0)
    {
        printf("\nThere is no message\n");
    }
    else
    {
        int index = 1;
        int size = queue_size(queue);

        printf("\nreceived messages:\n\n");

        while (size > 0)
        {
            char *message = get_queue(queue);

            printf("%d. %s\n", index, message);

            pop(queue);

            size = queue_size(queue);

            index++;
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    Queue *messagesQueue = create_queue();
    char *token = '\0';

    char iBuf[MAXDATASIZE + 1];
    char oBuf[MAXDATASIZE + 1];
    struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information
    int myPort;

    signal(1, sigHandler);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigControlC);

    state = IDLE;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Hostname> <Port Number>\n", argv[0]);
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL)
    { // get the host info
        perror("gethostbyname");
        exit(1);
    }

    myPort = atoi(argv[2]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(myPort); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct

    printf("Going to be connected ....\n");

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        fprintf(stderr, "connect: ERRNO %d\n", errno);
        exit(1);
    }

    printf("Connected ....\n");
    memset(iBuf, 0, MAXDATASIZE);
    memset(oBuf, 0, MAXDATASIZE);
    while (1)
    {
        bool ultimaOpcaoEscolhidaFoi4 = strcmp(oBuf, "4") == 0; //
        memset(oBuf, 0, MAXDATASIZE);
        char choosenOption;

        // deve pegar os dados do usuário aqui
        char *trash = get_user_input(iBuf, token, ultimaOpcaoEscolhidaFoi4);
        strcpy(oBuf, trash);

        choosenOption = oBuf[0];

        // opção de receber mensagem -> mostrar mensagens na fila
        if (choosenOption == '3' && strlen(token) > 0)
        {
            showMessages(messagesQueue);
        }

        oBuf[strlen(oBuf)] = 0;

        write(sockfd, oBuf, strlen(oBuf));

        printf("\n\tWaiting server....\n");
        memset(iBuf, 0, MAXDATASIZE);
        if ((numbytes = read(sockfd, iBuf, MAXDATASIZE)) > 0)
        {
            if (choosenOption == '0') // opção de login -> armazena o token retornado
            {
                token = iBuf;
            }
            else if (choosenOption == '1') // opção de logout -> reseta token
            {
                token = '\0';
            }
            else if (strcmp("logout performed", iBuf) != 0 && strcmp("sent", iBuf) != 0 && strcmp("not sent", iBuf) != 0) // recebeu uma mensagem
            {
                push(messagesQueue, iBuf);
            }

            printf("\n\tReceived [%s] with %d bytes\n", iBuf, numbytes);
        }
        if (!strcmp(iBuf, "quit"))
        {
            printf("Connection closed\n");
            close(sockfd);
            exit(0);
        }
        free(trash);
    }
}
