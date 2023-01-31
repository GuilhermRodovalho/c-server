Server = server.o
Client = client.o 
OBJS = *.o

all: serverObject clientObject

serverObject: $(wildcard server/*.c)
	gcc -o $(Server) $^ 

clientObject: $(wildcard client/*.c)
	gcc -o $(Client) $^

run-server:
	./server.o 8000

run-client:
	./client.o localhost 8000 

# Configurações para rodar com o valgrind (valida sobre memory leaks)
allDebug: serverDebug clientDebug

serverDebug: $(wildcard server/*.c)
	gcc -o $(Server) $^ -g -O0

clientDebug: $(wildcard client/*.c)
	gcc -o $(Client) $^ -g -O0

valgrind-client:
	valgrind --leak-check=yes -s ./client.o localhost 8000

valgrind-server:
	valgrind --leak-check=yes -s ./server.o 8000

# server: $(OBJS)
# 	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) 
