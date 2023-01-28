Server = server.o
Client = client.o 
OBJS = *.o

all: serverObject clientObject

serverObject: $(wildcard server/*.c)
	gcc -o $(Server) $^ 

clientObject: $(wildcard client/*.c)
	gcc -o $(Client) $^

# server: $(OBJS)
# 	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) 
