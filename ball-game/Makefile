CC=gcc

all: server client create

create: 
	mkdir -p client-src server-src

server: server.c tool.c
	$(CC) -g -o server server.c tool.c -lpthread

client: client.c tool.c
	$(CC) -g -o client client.c tool.c -lpthread

clean:
	rm -v -rf server client server-src client-src result-*.txt	