Server = server.o
Client = client.o 
OBJS = *.o

serverObject: $(wildcard server/*.c)
	gcc -o $(Server) $^ 
	make clientObject

clientObject: $(wildcard client/*.c)
	gcc -o $(Client) $^

# server: $(OBJS)
# 	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) 
