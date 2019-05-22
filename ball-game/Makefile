CC=gcc

all: server client create

create: 
	mkdir -p client-src server-src

server: server.c tool.c queue.c
	$(CC) -o server server.c tool.c queue.c -lpthread

client: client.c tool.c queue.c
	$(CC) -o client client.c tool.c queue.c -lpthread

clean:
	rm -v -rf server client server-src client-src result-*.txt	