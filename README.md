# c-server

A server built in C.
It's not complete. Shouldn't be used the way it is.

## Login

O client envia ao server uma mensagem no formato

`identificador_func_login|usuario|password`

Ações do server:

1. devolver o token na base 64
2. Salvar no arquivo uma string no formato:  
   token|endereco|flag_on, caso não exista para o endereço solicitado, caso exista alterar a flag de off para on

## Logout

O client envia uma mensagem no seguinte formato

Identificador_func_logout|token

O Server vai no arquivo, e altera a string, setando para

token|endereco|flag_off

## Envio de mensagem

O client envia ao server a mensagem no formato

`identificador_func_msg|token|endereco_destino|msg`

O server vai no arquivo e verifica se para aquele endereço de destino, o usuário está logado, se tiver envia a mensagem para o mesmo.

Acho q não vale a gente se preocupar de armazenar as mensagens para quem estar off-line para enviar ao fazer login

# Listar usuário

O client envia ao server o código "4"

O sever retorno uma lista de usuários no seguinte formato
[nome senha|endereço\n]

## Todo:

- [x] Mudar a flag de offline para online
- [x] Corrigir bug de fazer login duas vezes e apagar os dados
- [x] Corrigir logout
- [ ] funcionalidade de enviar mensagem
- [x] interface do cliente
