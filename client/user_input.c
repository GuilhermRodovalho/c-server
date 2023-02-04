#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "user_input.h"

char *get_login();
void append_with_pipe(char *dest, char *source);
char *show_not_logged_options();
char *show_logged_options(char *last_server_response);
char *build_logout_request(char *last_server_response);
char *build_message_request(char *token);

/**
 * @brief Esta função é responsável por exibir uma mensagem de saída para o usuário, baseado em sua resposta anterior do servidor, seu token atual e uma flag indicando se a resposta do servidor deve ser exibida.
 *
 * @param last_server_response A resposta do servidor anterior.
 * @param token O token atual do usuário.
 * @param mostrarResposta Flag indicando se a resposta do servidor deve ser exibida.
 *
 * @return char* Retorna uma string representando o que deve ser enviado para o servidor
 */
char *get_user_input(char *last_server_response, char *token, bool mostrarResposta)
{
    if (mostrarResposta)
    {
        printf("Usuários:\n");
        char currentChar;
        int i = 0;
        while (currentChar = last_server_response[i])
        {
            printf("%c", currentChar);

            i++;
        }
    }

    if (strnlen(last_server_response, 2) == 0 || strcmp("logout performed", last_server_response) == 0)
    {
        return show_not_logged_options();
    }
    else
    {
        /*
        pode ser feita uma validação antes para verificar se o server
        mandou uma resposta de erro, e nem entrar aqui caso seja erro. Logo
        poderemos saber que o que tem na resposta é com certeza o token do
        usuário
        */
        return show_logged_options(token);
    }
}

char *show_not_logged_options()
{
    char *output;
    int option;
    do
    {
        printf("\nMenu:\n");
        printf("1. Login\n");
        printf("2. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);
        getc(stdin); // tira o \n que ficou no buffer
        switch (option)
        {
        case 1:
            output = get_login();
            return output;
            break;
        case 2:
            return "quit";
            break;
        default:
            printf("Invalid option, please try again\n");
            break;
        }
    } while (option > 2 || option < 1);
}

char *show_logged_options(char *token)
{
    char *output;
    int option;
    do
    {
        printf("\nMenu:\n");
        printf("1. Logout\n");
        printf("2. Send message\n");
        printf("3. Receive messages\n");
        printf("4. List all users\n");
        printf("Enter your choice: ");
        scanf("%d", &option);
        getc(stdin); // tira o \n que ficou no buffer
        switch (option)
        {
        case 1:
            return build_logout_request(token);
            break;
        case 2:
            return build_message_request(token);
            break;
        case 3:
            return "3|receive_messages";
            break;
        case 4:
            char *ponteiroNecessario = calloc(2, 1);
            strcat(ponteiroNecessario, "4");
            return ponteiroNecessario;
        default:
            printf("Invalid option, please try again\n");
            break;
        }
    } while (option > 2 || option < 1);

    return "quit";
}

char *get_login()
{
    char *response = malloc(300 * sizeof(char));
    memset(response, 0, 300);
    char recipientAddress[100], message[100];
    memset(recipientAddress, 0, 100);
    memset(message, 0, 100);

    printf("Login option selected\n");
    printf("User: ");
    fgets(recipientAddress, 100, stdin);
    printf("Password: ");
    fgets(message, 100, stdin);

    recipientAddress[strlen(recipientAddress) - 1] = 0;
    message[strlen(message) - 1] = 0;

    append_with_pipe(response, "0");
    append_with_pipe(response, recipientAddress);
    strcat(response, message);

    return response;
}

char *build_logout_request(char *token)
{
    char *response = calloc(strlen(token) + 3, sizeof(char));
    append_with_pipe(response, "1");
    strcat(response, token);
    return response;
}

char *build_message_request(char *token)
{
    char *response = malloc(360 * sizeof(char));
    memset(response, 0, 360);

    char recipientAddress[100], message[160];
    memset(recipientAddress, 0, 100);
    memset(message, 0, 160);

    printf("Send message option selected\n");
    printf("Recipient's address: ");
    fgets(recipientAddress, 100, stdin);
    printf("Message: ");
    fgets(message, 160, stdin);

    recipientAddress[strlen(recipientAddress) - 1] = 0;
    message[strlen(message) - 1] = 0;

    append_with_pipe(response, "2");
    append_with_pipe(response, token);
    append_with_pipe(response, recipientAddress);
    strcat(response, message);

    printf("%s\n", response);

    return response;
}

/**
 * @brief
 * Append source to dest and a pipe after it
 *
 */
void append_with_pipe(char *dest, char *source)
{
    strcat(dest, source);
    strcat(dest, "|");
}