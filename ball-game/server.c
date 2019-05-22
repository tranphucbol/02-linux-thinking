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

#include "queue.h"
#include "tool.h"

#define TRUE 1
#define FALSE 0

#define OUT_OF_STOCK 0
#define SUBMIT 1

void *connection_handler(void *socket_desc);
void *event_handler(void *args);

struct CLIENT {
    int sock;
    int sum;
};

pthread_mutex_t lockCount, lockBalls, lockIndex;
int itr = 0;
int nBall = 10;
int *balls = NULL;
int countClient = 0;
struct CLIENT clients[MAX_CLIENT];
struct Queue *q;
int outOfStock = 0;
int submit = 0;

int main(int argc, char *argv[])
{
    int opt = TRUE;
    int master_socket, addrlen, new_socket;
    struct sockaddr_in server, client;

    //random number of ball
    if(argc >= 2) {
        nBall = atoi(argv[1]);
    } else 
        nBall = rand() % 901 + 100;

    balls = (int *)malloc(nBall * sizeof(int));

    for (int i = 0; i < nBall; i++)
    {
        balls[i] = rand() % 10000;
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

    pthread_t tid, t_out;
    pthread_create(&t_out, NULL, event_handler, NULL);

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

        
        
        
        int * index = (int *)malloc(sizeof(int));

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
            return 1;
        }
    }

    free(balls);

    return 0;
}

int compare(const void *a, const void *b)
{
    return (((struct CLIENT *)b)->sum - ((struct CLIENT *)a)->sum);
}

void *event_handler(void *args)
{
    char buffer[1024];
    q = createQueue();
    int countab = 0;
    while (TRUE)
    {
        while (q->front != NULL)
        {
            struct QNode *n = deQueue(q);
            switch (n->key)
            {
            case OUT_OF_STOCK:
                if (outOfStock == 0)
                {
                    char buffer[1024];
                    for (int i = 0; i < MAX_CLIENT; i++)
                        if (clients[i].sock > 0)
                        {
                            buffer[0] = CODE_FILE;
                            // send(clients[i].sock, buffer, 1024, 0);
                            // strcpy(buffer, "Out of stock");
                            int ok = send(clients[i].sock, buffer, 1024, 0);
                            countab++;
                            printf("socket: %d, ok: %d\n", clients[i].sock, ok);
                        }
                    printf("count: %d\n", countab);
                }
                outOfStock = 1;
                break;
            case SUBMIT:
                submit++;
                printf("submit: %d\n", submit);
                // printf("count: %d\n", countab);
                if(submit == countab) {
                    qsort(clients, 10, sizeof(struct CLIENT), compare);
                    memset(buffer, 0, 1024);
                    buffer[0] = CODE_RESULT;
                    for(int i=0; i<MAX_CLIENT; i++) {
                        if(clients[i].sock > 0) {
                            char tmp[1024];
                            sprintf(tmp, "Sock: %d, Sum: %d\n", clients[i].sock, clients[i].sum);
                            strcat(buffer + 1, tmp);
                            printf("Sock: %d, Sum: %d\n", clients[i].sock, clients[i].sum);
                        }
                    }

                    for(int i=0; i<MAX_CLIENT; i++) {
                        if(clients[i].sock > 0) {
                            send(clients[i].sock, buffer, strlen(buffer + 1) + 1, 0);
                        }
                    }
                }
                break;
            default:
                break;
            }
            free(n);
        }
    }
    return 0;
}

void *connection_handler(void *index)
{
    int iClient = *(int *)index;

    int valread;

    char buffer[1024];

    //send 
    strcpy(buffer, "Welcome to Ballhub");
    send(clients[iClient].sock, buffer, 1024, 0);

    //send socket
    memset(buffer, 0, 1024);
    buffer[0] = CODE_SOCKET;
    encode(clients[iClient].sock, buffer + 1);
    // encode(iClient, buffer + 1);
    send(clients[iClient].sock, buffer, 1024, 0);

    memset(buffer, 0, sizeof(buffer));

    while ((valread = read(clients[iClient].sock, buffer, 1024)) > 0)
    {
        buffer[valread] = '\0';
        pthread_mutex_lock(&lockBalls);
        if (itr < nBall && strcmp(buffer, "get") == 0)
        {
            char val[4];

            
            int ball = balls[itr++];
            

            encode(ball, val);
            buffer[0] = CODE_VALUE;
            memcpy(buffer + 1, val, 4);
            // printf("sent to sock %d ball %d\n", clients[iClient].sock, itr);
            send(clients[iClient].sock, buffer, 1024, 0);
        }
        else if (itr == nBall && strcmp(buffer, "get") == 0)
        {
            enQueue(q, OUT_OF_STOCK);
            buffer[0] = CODE_OUT_OF_STOCK;
            send(clients[iClient].sock, buffer, 1, 0);
        }
        else if (buffer[0] == CODE_FILE)
        {
            getFile(buffer + 1, "server-src/");

            char filename[50];
            sprintf(filename, "server-src/%d", clients[iClient].sock);
            FILE * fp = fopen(filename, "rb");

            int nB = 0;
            int * cBall = NULL;

            fread(&nB, sizeof(int), 1, fp);
            cBall = (int*)malloc(nB * sizeof(int));
            fread(cBall, sizeof(int) * nB, 1, fp);
            fclose(fp);

            // printf("array of sock %d\n", clients[iClient].sock);
            for(int i=0; i<nB; i++) {
                // printf("%d ", cBall[i]);
                clients[iClient].sum += cBall[i];
            }
            // printf("\n");

            enQueue(q, SUBMIT);

            free(cBall);
        }
        pthread_mutex_unlock(&lockBalls);
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
    return 0;
}