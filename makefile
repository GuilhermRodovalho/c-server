S = server.o
C = client.o 

server: $(wildcard server/*.c)
	gcc -o $S $^ 

client: $(wildcard client/*.c)
	gcc -o $C $^

# server: $(OBJS)
# 	gcc $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) 
