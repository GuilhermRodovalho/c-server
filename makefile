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

# server: $(OBJS)
# 	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) 
