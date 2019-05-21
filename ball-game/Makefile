CC=gcc

all: server client

server: server.c tool.c queue.c
	$(CC) -o server server.c tool.c queue.c -lpthread

client: client.c tool.c queue.c
	$(CC) -o client client.c tool.c queue.c -lpthread	