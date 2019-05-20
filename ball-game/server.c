#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define MAX_CLIENT 10

#define CODE_VALUE 1

void encode(int n, char buffer[]);
int decode(char buffer[]);
void *connection_handler(void *socket_desc);

pthread_mutex_t lockCount, lockBalls;
int itr = 0;
int nBall  = 20;
int *balls = NULL;
int countClient = 0;

int main(int argc, char *argv[])
{
    int opt = TRUE;
    int master_socket, addrlen, new_socket;
    struct sockaddr_in server, client;

    //random number of ball
    nBall = rand() % 901 + 100;

    balls = (int *)malloc(nBall * sizeof(int));

    for (int i = 0; i < nBall; i++)
    {
        balls[i] = rand() % 10000;
    }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(server);
    puts("Waiting for connections ...");

    pthread_t tid;

    while ((new_socket = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen)))
    {
        char buffer[1024];
        if(countClient == MAX_CLIENT) {
            strcpy(buffer, "Full slot");
            send(new_socket, buffer, sizeof(buffer), 0);
            close(new_socket);
            continue;
        }

        countClient++;

        puts("Connections accepted");

        if (pthread_create(&tid, NULL, connection_handler, (void *)&new_socket) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    free(balls); 

    return 0;
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    int valread;

    char buffer[1024];
    strcpy(buffer, "Welcome to Ballhub");
    send(sock, buffer, 1024, 0);
    memset(buffer, 0, sizeof(buffer));

    while ((valread = read(sock, buffer, 1024)) > 0)
    {
        buffer[valread] = '\0';
        if (itr < nBall && strcmp(buffer, "get") == 0)
        {
            char val[8];

            pthread_mutex_lock(&lockBalls);
            int ball = balls[itr++];
            pthread_mutex_unlock(&lockBalls);

            encode(ball, val);
            buffer[0] = CODE_VALUE;
            memcpy(buffer + 1, val, 8);
            send(sock, buffer, 1024, 0);
        }

        if (itr == nBall)
        {
            strcpy(buffer, "Out of stock");
            send(sock, buffer, 1024, 0);
        }
    }

    printf("Host disconnected socket: %d\n", sock);

    //Close the socket and mark as 0 in list for reuse
    close(sock);
    
    pthread_mutex_lock(&lockCount);
    countClient--;
    pthread_mutex_unlock(&lockCount);

    return 0;
}

void encode(int n, char buffer[])
{
    for (int i = 0; i < 8; i++)
    {
        int tmp = (n >> (i * 4) & 0xf);
        buffer[i] = tmp;
    }
}

int decode(char buffer[])
{
    int n = 0;
    for (int i = 0; i < 8; i++)
    {
        n = (buffer[i] << (i * 4)) | n;
    }
    return n;
}