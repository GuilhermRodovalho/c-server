#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "helper.h"

const char *login_file = "login_file.txt";

char *register_user(char *buf);
int save_user_to_file(char *username, char *password);
int should_save_user_to_file(char *username);

/**
 * @brief
 * Função que lida com o request feito pelo usuário
 *
 * @param buf input do client
 * '0' (decimal 48) = função de registro
 * '1' (decimal 49) = função de login
 * @return char* resposta
 */
char *handle_request(char *buf)
{
    // funcao requisitada
    int function = (int)buf[0];

    // desconsidera o primeiro caractere
    buf++;

    char *result = NULL;

    switch (function)
    {
    case 48:
        result = register_user(buf);
        break;

    default:
        break;
    }

    return result;
}

/**
 * @brief
 * Cria um usuário, seguindo o format `username senha`
 *
 * @param buf input do client.
 * @return char* em caso de sucesso "registrado", "falha" em caso de falha
 */
char *register_user(char *buf)
{
    char *username = get_first_word(buf);
    char *password = get_next_word(buf);

    if (should_save_user_to_file(username) != 0)
    {
        return "login already in use";
    }

    if (save_user_to_file(username, password) == 1)
    {
        printf("deu ruim escrevendo no arquivo");
    }
}

// 0 se deu tudo certo, 1 se deu erro
int save_user_to_file(char *username, char *password)
{
    FILE *user_file = fopen(login_file, "a");
    if (user_file == NULL)
    {
        printf("error opening the file\n");
        return 1;
    }
    fprintf(user_file, "%s\n%s\n", username, password);

    return 0;
}

// retorna 0 se deve salvar o usuário no arquivo
// 1 caso não deva (já existe outro usuário com esse login)
// 2 em caso de erro
int should_save_user_to_file(char *username)
{
    char buffer[100];
    if (access(login_file, F_OK) != 0) // checa se o arquivo existe (primeiro usuário)
    {
        return 0;
    }

    FILE *logins_file = fopen(login_file, "r");
    if (logins_file == NULL)
    {
        printf("error opening the file\n");
        return 2;
    }

    while (fgets(buffer, 100, logins_file) != NULL)
    {
        // remove \n no final
        int len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = 0;

        if (strcmp(buffer, username) == 0)
        {
            return 1;
        }

        // joga fora a senha
        // nao deveria entrar nesse if pq sempre tem que ser em pares (usuario e senha)
        if (fgets(buffer, 100, logins_file) == NULL)
        {
            return 2;
        }
    }

    return 0;
}
