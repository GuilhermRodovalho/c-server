#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user_input.h"

char *get_login();
void append_with_pipe(char *dest, char *source);
char *show_not_logged_options();
char *show_logged_options(char *last_server_response);
char *build_logout_request(char *last_server_response);

char *get_user_input(char *last_server_response)
{
    if (strnlen(last_server_response, 2) == 0 || strcmp("deu bom", last_server_response) == 0)
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
        return show_logged_options(last_server_response);
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

char *show_logged_options(char *response)
{
    char *output;
    int option;
    do
    {
        printf("\nMenu:\n");
        printf("1. Logout\n");
        printf("2. Send message\n");
        printf("3. Receive messages\n");
        printf("Enter your choice: ");
        scanf("%d", &option);
        getc(stdin); // tira o \n que ficou no buffer
        switch (option)
        {
        case 1:
            output = build_logout_request(response);
            return output;
            break;
        case 2:
            // TODO

            return "quit";
            break;
        case 3:
            // TODO
            return "quit";
            break;
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
    char username[100], password[100];
    memset(username, 0, 100);
    memset(password, 0, 100);

    printf("Login selecionado\n");
    printf("Usuário: ");
    fgets(username, 100, stdin);
    printf("Senha: ");
    fgets(password, 100, stdin);

    username[strlen(username) - 1] = 0;
    password[strlen(password) - 1] = 0;

    append_with_pipe(response, "0");
    append_with_pipe(response, username);
    strcat(response, password);

    return response;
}

char *build_logout_request(char *token)
{
    char *response = calloc(strlen(token) + 3, sizeof(char));
    append_with_pipe(response, "1");
    strcat(response, token);
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