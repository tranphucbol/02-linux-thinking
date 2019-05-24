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

#include "tool.h"

struct CLIENT
{
    int sock;
    int sum;
};

pthread_mutex_t lockCount, lockBalls, mSubmit, mOut;
int itr = 0;
int nBall = 10;
int *balls = NULL;
int countClient = 0;
struct CLIENT clients[MAX_CLIENT];
int submit = 0;
int countSubmit = 0;

int outOfStock = FALSE;

void *connection_handler(void *socket_desc);
void *submit_handler(void *argv);
void sendAll(char buffer[]);

int main(int argc, char const *argv[])
{
    int opt = TRUE;
    int master_socket, addrlen, new_socket;
    struct sockaddr_in server, client;

    //random number of ball
    if (argc >= 2)
    {
        nBall = atoi(argv[1]);
    }
    else
        nBall = rand() % 901 + 100;

    balls = (int *)malloc(nBall * sizeof(int));

    for (int i = 0; i < nBall; i++)
    {
        balls[i] = rand() % 10;
    }

    for (int i = 0; i < MAX_CLIENT; i++)
    {
        clients[i].sock = 0;
        clients[i].sum = 0;
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
    if (listen(master_socket, 100) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(server);
    puts("Waiting for connections ...");
    pthread_t tid, t_submit;

    while ((new_socket = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen)))
    {
        char buffer[1024];
        pthread_mutex_lock(&lockCount);
        if (countClient == MAX_CLIENT)
        {
            strcpy(buffer, "Full slot");
            send(new_socket, buffer, sizeof(buffer), 0);
            close(new_socket);
            continue;
        }
        countClient++;
        pthread_mutex_unlock(&lockCount);

        printf("Connections accepted, socket: %d\n", new_socket);

        int *index = (int *)malloc(sizeof(int));

        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clients[i].sock == 0)
            {
                *index = i;
                clients[i].sock = new_socket;
                break;
            }
        }

        if (pthread_create(&tid, NULL, connection_handler, index) < 0)
        {
            pthread_mutex_lock(&lockCount);
            countClient--;
            pthread_mutex_unlock(&lockCount);
            clients[*index].sock = 0;
            perror("Could not create thread");
            // return 1;
        }
    }

    free(balls);
    close(master_socket);

    return 0;
}

int compare(const void *a, const void *b)
{
    return (((struct CLIENT *)b)->sum - ((struct CLIENT *)a)->sum);
}

void *connection_handler(void *index)
{
    int iClient = *(int *)index;
    int valread;
    char buffer[1024];

    //send socket number
    memset(buffer, 0, 1024);
    buffer[0] = CODE_SOCKET;
    // encode(clients[iClient].sock, buffer + 1);
    sprintf(buffer + 1, "%d", clients[iClient].sock);
    send(clients[iClient].sock, buffer, 1024, 0);

    // memset(buffer, 0, 1024);
    // buffer[0] = CODE_MSG;
    // strcpy(buffer + 1, "Welcome to Ballhub");
    // send(clients[iClient].sock, buffer, 1024, 0);

    memset(buffer, 0, 1024);

    while ((valread = read(clients[iClient].sock, buffer, 1024)) > 0)
    {
        switch (buffer[0])
        {
        case CODE_GET:
            pthread_mutex_lock(&lockBalls);
            if (itr < nBall)
            {
                char val[4];
                int ball = balls[itr++];
                encode(ball, val);
                buffer[0] = CODE_VALUE;
                memcpy(buffer + 1, val, 4);
                send(clients[iClient].sock, buffer, 1024, 0);
            }
            else if (itr == nBall)
            {
                // buffer[0] = CODE_OUT_OF_STOCK;
                // send(clients[iClient].sock, buffer, 1024, 0);
                
                pthread_mutex_lock(&mOut);
                if(outOfStock == FALSE) {
                    outOfStock = TRUE;
                    pthread_t t_submit;
                    pthread_create(&t_submit, NULL, submit_handler, NULL);
                    for(int i=0; i<MAX_CLIENT; i++) {
                        if(clients[i].sock > 0) {
                            buffer[0] = CODE_OUT_OF_STOCK;
                            send(clients[i].sock, buffer, 1024, 0);
                            buffer[0] = CODE_FILE;
                            send(clients[i].sock, buffer, 1024, 0);
                        }
                    }
                }
                pthread_mutex_unlock(&mOut);
                
            }
            pthread_mutex_unlock(&lockBalls);
            break;
        case CODE_FILE:
        {
            getFile(buffer + 1, "server-src/");
            char filename[50];
            sprintf(filename, "server-src/%d", clients[iClient].sock);
            FILE *fp = fopen(filename, "rb");

            int nB = 0;
            int *cBall = NULL;

            fread(&nB, sizeof(int), 1, fp);
            cBall = (int *)malloc(nB * sizeof(int));
            fread(cBall, sizeof(int) * nB, 1, fp);
            fclose(fp);

            // printf("array of sock %d\n", clients[iClient].sock);
            for (int i = 0; i < nB; i++)
            {
                // printf("%d ", cBall[i]);
                clients[iClient].sum += cBall[i];
            }

            pthread_mutex_lock(&mSubmit);
            submit++;
            pthread_mutex_unlock(&mSubmit);
            free(cBall);
        }
        break;
        default:
            break;
        }
        memset(buffer, 0, 1024);
    }

    printf("Host disconnected socket: %d\n", clients[iClient].sock);

    //Close the socket and mark as 0 in list for reuse
    close(clients[iClient].sock);

    pthread_mutex_lock(&lockCount);
    countClient--;
    pthread_mutex_unlock(&lockCount);

    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].sock == clients[iClient].sock)
        {
            clients[i].sock = 0;
        }
    }

    free(index);
}

void *submit_handler(void *argv)
{
    char buffer[1024];
    while (TRUE)
    {
        pthread_mutex_lock(&mSubmit);
        if (submit == countClient)
        {
            qsort(clients, 10, sizeof(struct CLIENT), compare);
            memset(buffer, 0, 1024);
            buffer[0] = CODE_RESULT;
            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (clients[i].sock > 0)
                {
                    char tmp[1024];
                    sprintf(tmp, "Sock: %d, Sum: %d\n", clients[i].sock, clients[i].sum);
                    strcat(buffer + 1, tmp);
                    printf("Sock: %d, Sum: %d\n", clients[i].sock, clients[i].sum);
                }
            }

            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (clients[i].sock > 0)
                {
                    send(clients[i].sock, buffer, 1024, 0);
                }
            }
            pthread_mutex_unlock(&mSubmit);
            return 0;     
        }
        pthread_mutex_unlock(&mSubmit);
    }
    return 0;
}

void sendAll(char buffer[])
{
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].sock > 0)
        {
            send(clients[i].sock, buffer, 1024, 0);
        }
    }
}