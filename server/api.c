#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "helper.h"
#include "base64.h"

const char *DATA_FILE = "data_file.txt";

char *login(char *buf);
char *decode_testing(char *buf);
char *encoder_helper(char *username, char *password);
int save_token_to_file(char *token, int is_logged);
int check_if_token_exists(char *username);

/**
 * @brief
 * Função que lida com o request feito pelo usuário
 *
 * @param buf input do client
 * '0' (decimal 48) = função de login. Registra e retorna o token de acesso
 * '1' (decimal 49) = função
 * @return char* resposta
 */
char *handle_request(char *buf, struct sockaddr_in *adress)
{
    // funcao requisitada
    int function = (int)buf[0];

    printf("Função pedida: %d\n", function);
    printf("Dados do enviador\n");
    printf("\tporta: %d\n", adress->sin_port);
    printf("\tendereço: %d\n", adress->sin_addr.s_addr);
    printf("\tzero?: %u\n", adress->sin_zero[0]);

    printf("endereço convertido: %s\n", inet_ntoa(adress->sin_addr));

    // desconsidera o primeiro caractere
    buf++;

    char *result = NULL;

    switch (function)
    {
    case 48:
        result = login(buf);
        break;
    case 49:
        result = decode_testing(buf);
        break;

    default:
        break;
    }

    return result;
}

/**
 * @brief
 * Cria um token de usuário, seguindo o formato `username senha`
 *
 * @param buf input do client (usuario e senha).
 * @return char* token. Contém o token gerado pela aplicação.
 */
char *login(char *buf)
{
    // separa a entrada em tokens divididos por |
    char *username = strtok(buf, "|");
    char *password = strtok(NULL, "|");

    printf("username: %s\npass: %s\n", username, password);

    char *token = encoder_helper(username, password);

    printf("token: %s\n", token);

    if (check_if_token_exists(token) != 0)
    {
        // TODO
        // deve alterar a flag para on
        return "User is already logged in";
    }

    save_token_to_file(token, 1);

    return token;
}

/**
 * @brief
 * Encoda em base64 a string no format "username password" e retorna o token
 * Não é completamente seguro, pois o encodign pode ser revertido
 *
 *
 * @param username usuário passado pelo client
 * @param password senha
 * @return char* token base64 gerado
 */
char *encoder_helper(char *username, char *password)
{
    char *userpass = malloc(strlen(username) + strlen(password) + 1);
    strcpy(userpass, username);
    strcat(userpass, " ");
    strcat(userpass, password);

    size_t input_length = strlen(userpass);
    size_t output_length;

    char *encoded = base64_encode((unsigned char *)userpass, input_length, &output_length);

    return encoded;
}

char *decode_testing(char *buf)
{

    size_t decoded_length;
    char *decoded = base64_decode(buf, strlen(buf), &decoded_length);

    printf("%s\n", decoded);
    return decoded;
}

// retorna 0 se o token nao existir no arquivo
// 1 caso já exista
// 2 em caso de erro
int check_if_token_exists(char *token)
{
    char buffer[300];
    if (access(DATA_FILE, F_OK) != 0) // checa se o arquivo existe (primeiro usuário)
    {
        return 0;
    }

    FILE *tokens_file = fopen(DATA_FILE, "r");
    if (tokens_file == NULL)
    {
        printf("error opening the file\n");
        return 2;
    }

    while (fgets(buffer, 300, tokens_file) != NULL)
    {
        // remove \n no final
        int len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = 0;

        char *token = strtok(buffer, "|");
        if (strcmp(buffer, token) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief
 * Salva o char *token no arquivo, com a flag is_logged.
 *
 * @param token
 * @param is_logged 1 para logado, 0 para deslogado
 * @return int 0 se deu certo, 1 se teve problema
 */
int save_token_to_file(char *token, int is_logged)
{
    FILE *user_file = fopen(DATA_FILE, "a");
    if (user_file == NULL)
    {
        printf("error opening the file\n");
        return 1;
    }
    // coloca um ao salvar
    fprintf(user_file, "%s|%d", token, is_logged);

    return 0;
}
