#include <stdio.h>
#include <string.h>

char *register_user(char *buf);
char *get_first_word(char *str);
char *get_next_word(char *str);

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
}

char *get_first_word(char *str)
{
    char *first_word = str;
    int i = 0;
    while (str[i] != ' ' && str[i] != '\0')
    {
        i++;
    }
    str[i] = '\0';
    return first_word;
}

char *get_next_word(char *str)
{
    int i = 0;
    while (str[i] != ' ' && str[i] != '\0')
    {
        i++;
    }
    while (str[i] == ' ' || str[i] == '\0')
    {
        i++;
    }
    return &str[i];
}
