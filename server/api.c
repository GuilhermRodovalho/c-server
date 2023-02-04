#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../helpers/helper.h"
#include "base64.h"

const char *DATA_FILE = "data_file.txt";

char *login(char *buf, const struct sockaddr_in *adress);
char *logout(char *token);
char *sendMessage(char *buf, int sockfd);
char *decode(char *buf);
char *encoder_helper(char *username, char *password);
int save_data_to_file(const char *token, const struct sockaddr_in *adress, int is_logged);
int save_raw_data_to_file(char *data);
int check_if_token_exists(const char *username);
char *delete_line_from_file(const char *file_name, const char *line_start);
int is_recipient_online(const char *recipientAddress);
char *returnOnlineUsers();

/**
 * @brief
 * Função que lida com o request feito pelo usuário
 *
 * @param buf input do client
 * '0' (decimal 48) = função de login. Registra e retorna o token de acesso
 * '1' (decimal 49) = função de logout
 * '2' (decimal 50) = função de enviar msg
 * '3' (decimal 51) = função de receber ms
 * '4' (decimal 52) = função de listar usuários
 * @param adress estrutura com as informações do endereço do cliente
 * @param sockfd socket file descriptor
 * @return char* resposta
 */
char *handle_request(char *buf, const struct sockaddr_in *adress, int sockfd)
{
        // funcao requisitada
        int function = (int)buf[0];

        // printf("Função pedida: %d\n", function);
        // printf("Dados do enviador\n");
        // printf("\tporta: %d\n", adress->sin_port);
        // printf("\tendereço: %d\n", adress->sin_addr.s_addr);
        // printf("\tzero?: %u\n", adress->sin_zero[0]);

        // printf("endereço convertido: %s\n", inet_ntoa(adress->sin_addr));

        char *result = buf;
        // desconsidera o primeiro caractere (o ideal seria criar outra variável)
        buf++;

        switch (function)
        {
        case 48:
                result = login(buf, adress);
                break;
        case 49:
                result = logout(buf);
                break;
        case 50:
                result = sendMessage(buf, sockfd);
                break;

        case 52:
                result = returnOnlineUsers();
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
 * @param adress estrutura com as informações do endereço do cliente
 * @return char* token. Contém o token gerado pela aplicação.
 */
char *login(char *buf, const struct sockaddr_in *adress)
{
        // separa a entrada em tokens divididos por |
        char *username = strtok(buf, "|");
        char *password = strtok(NULL, "|");

        char *token = encoder_helper(username, password);

        if (check_if_token_exists(token) != 0)
        {
                /* caso o token já esteja no arquivo, basicamente deleta do arquivo (
                provavelmente estava com a flag 0) e insere novamente com a flag 1 para
                indicar que agora está logado.
                */

                char *trash = delete_line_from_file(DATA_FILE, token);
                free(trash);
        }

        save_data_to_file(token, adress, 1);

        return token;
}

/**
 * @brief
 * Desfaz o processo de login, marcando o usuário como offline
 *
 * @param buf token de usuário começando com |
 * @return char* "logout realizado"
 */
char *logout(char *buf)
{
        char *token = strtok(buf, "|");
        char *line = delete_line_from_file(DATA_FILE, token);

        // Muda o 1 para 0 para setar offline
        line[strlen(line) - 2] = '0';

        save_raw_data_to_file(line);

        free(line);

        return "logout performed";
}

/**
 * @brief
 * Envia uma mensagem a um usuário específico
 *
 * @param buf mensagem a ser enviada. Deve conter o destinatário no formato `recipient_address|message`
 * @param sockfd socket file descriptor
 * @return char* mensagem de sucesso ou fracasso na operação
 */
char *sendMessage(char *buf, int sockfd)
{
        buf++;

        char *address;
        char *message;

        char *aux = strtok(buf, "|");

        aux = strtok(NULL, "|");
        address = aux;

        aux = strtok(NULL, "|");
        message = aux;

        if (is_recipient_online(address) == 1)
        {
                char *s_addr;
                char *port;

                char *temp = strtok(address, ":");
                s_addr = temp;

                temp = strtok(NULL, ":");
                port = temp;

                struct in_addr sin_addr;
                sin_addr.s_addr = (in_addr_t)atoi(s_addr);

                struct sockaddr_in recipient_addr;
                recipient_addr.sin_family = AF_INET;
                recipient_addr.sin_port = (unsigned short)strtoul(port, NULL, 0);
                recipient_addr.sin_addr = sin_addr;
                memset(&(recipient_addr.sin_zero), '\0', 8);

                printf("Try to send the message %s to user's port %d and user's address %d\n", message, recipient_addr.sin_port, recipient_addr.sin_addr.s_addr);

                socklen_t sin_size = sizeof(struct sockaddr_in);

                int new_fd, retVal;

                if ((new_fd = accept(sockfd, (struct sockaddr *)&recipient_addr, &sin_size)) == -1)
                {
                        printf("deu ruim\n");

                        perror("accept");

                        close(new_fd);

                        return "not sent";
                }
                else
                {
                        printf("Sending message...\n");

                        if ((retVal = write(new_fd, message, strlen(message))) == -1)
                        {
                                perror("send");
                                close(new_fd);
                                exit(-1);
                        }

                        close(new_fd);

                        return "sent";
                }
        }
}

/**
 * @brief
 * Auxiliar na geração de token. Concatena username e password com espaço
 *
 * @param username nome de usuário
 * @param password senha
 * @return char* token
 */
char *encoder_helper(char *username, char *password)
{
        unsigned char *userpass = malloc(strlen(username) + strlen(password) + 3);
        strcpy(userpass, username);
        strcat(userpass, " ");
        strcat(userpass, password);

        size_t input_length = strlen(userpass);
        size_t output_length;

        char *encoded = base64_encode(userpass, input_length, &output_length);

        encoded[output_length] = 0;

        free(userpass);

        return encoded;
}

/**
 * @brief
 * Decodifica uma mensagem em base64
 *
 * @param buf mensagem codificada
 * @return char* mensagem decodificada
 */
char *decode(char *buf)
{
        size_t decoded_length;
        char *decoded = base64_decode(buf, strlen(buf), &decoded_length);

        return decoded;
}

/**
 * @brief
 * verifica se um token existe no arquivo
 *
 * @param token a ser verificado
 * @return int 0 se o token não existir, 1 se existir, 2 em caso de erro na abertura do arquivo
 */
int check_if_token_exists(const char *token)
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

                char *line_token = strtok(buffer, "|");
                if (strcmp(line_token, token) == 0)
                {
                        return 1;
                }
        }

        fclose(tokens_file);

        return 0;
}

/**
 * @brief Verifica se um destinatário está online.
 *
 * @param recipientAddress Endereço do destinatário.
 * @return int Retorna 1 caso o destinatário esteja online, 0 caso não esteja e 2 em caso de erro.
 */
int is_recipient_online(const char *recipientAddress)
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

                char *address = strtok(buffer, "|");
                address = strtok(NULL, "|");

                if (strcmp(address, recipientAddress) == 0)
                {
                        if (buffer[strlen(buffer) - 2] = '1') // O destinatário está online
                                return 1;
                        return 0;
                }
        }

        fclose(tokens_file);

        return 0;
}

/**
 * @brief Função que retorna a lista de usuários online.
 *
 * @return char* String com os usuários online, separados por "|" e com quebra de linha.
 */

char *returnOnlineUsers()
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
                return NULL;
        }
        char *response = calloc(10000, 1);
        response[0] = 0;

        while (fgets(buffer, 300, tokens_file) != NULL)
        {
                char *token = strtok(buffer, "|");
                char *decoded_token = decode(token);
                char *address = strtok(NULL, "|");

                strcat(response, decoded_token);
                strcat(response, "|");
                strcat(response, address);
                strcat(response, "\n");
        }

        printf("response: %s\n", response);

        fclose(tokens_file);

        return response;
}

/**
 * @brief
 * Salva o char *token com endereço e is_logged no arquivo, com o seguindo o formato
 * `token|endereco:porta|is_logged`
 *
 * @param token
 * @param adress é a struct sockaddr_in do endereço do client
 * @param is_logged 1 para logado, 0 para deslogado
 * @return int 0 se deu certo, 1 se teve problema
 */
int save_data_to_file(const char *token, const struct sockaddr_in *adress, int is_logged)
{
        FILE *user_file = fopen(DATA_FILE, "a");
        if (user_file == NULL)
        {
                printf("error opening the file\n");
                return 1;
        }
        // coloca um ao salvar
        fprintf(user_file,
                "%s|%d:%d|%d\n",
                token,
                adress->sin_addr.s_addr,
                adress->sin_port,
                is_logged);

        fclose(user_file);

        return 0;
}

/**
 * @brief Salva os dados brutos em um arquivo
 *
 * @param data Dados a serem salvos
 * @return int Retorna 0 em caso de sucesso e 1 em caso de falha
 */
int save_raw_data_to_file(char *data)
{
        FILE *user_file = fopen(DATA_FILE, "a");
        if (user_file == NULL)
        {
                printf("error opening the file\n");
                return 1;
        }
        fputs(data, user_file);

        fclose(user_file);

        return 0;
}

/**
 * @brief
 * Remove do arquivo file_name qualquer linha que começa com char *line_start
 *
 * @param file_name nome do arquivo
 * @param line_start com o que deve comparar o início da linha
 * @return a última linha que não foi deletada. Note que várias linhas podem ser
 * deletadas caso comecem com line_start
 */
char *delete_line_from_file(const char *file_name, const char *line_start)
{
        char buffer[200];
        char tempFileName[] = "temp.txt";
        FILE *file = fopen(file_name, "r");
        FILE *tempFile = fopen(tempFileName, "w");

        char *last_deleted_line = (char *)calloc(200, sizeof(char));

        while (fgets(buffer, sizeof(buffer), file))
        {
                if (strlen(buffer) <= 1)
                        continue;

                if (strncmp(buffer, line_start, strlen(line_start)) != 0)
                {
                        fputs(buffer, tempFile);
                }
                else
                {
                        strcpy(last_deleted_line, buffer);
                }
        }

        fclose(file);
        fclose(tempFile);

        if (remove(file_name) != 0)
        {
                printf("aconteceu algo na hora de deletar o arquivo");
        }
        rename(tempFileName, file_name);

        return last_deleted_line;
}
