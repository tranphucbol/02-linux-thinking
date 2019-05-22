#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <pthread.h>
#include <time.h>

#include "tool.h"

int run_auto = 0;
pthread_mutex_t mutex, mfile;

void *inputThread(void *sockfd)
{
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        pthread_mutex_lock(&mutex);
        if(run_auto == 0) {
            scanf("%[^\n]%*c", buffer);
        } else {
            // sleep(1);
            nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
            strcpy(buffer, "get");
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mfile);
        write(*(int *)sockfd, buffer, sizeof(buffer));
        pthread_mutex_unlock(&mfile);
    }
    return 0;
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr;
    int valread;
    int itr = 0;
    int *balls = (int *)malloc(1000 * sizeof(int));

    srand((unsigned int)time(0));

    if(argc >= 2) {
        run_auto = 1;
    }

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket creation failed");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&servaddr,
                sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return -1;
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, inputThread, &sockfd);

    char filename[50];
    FILE *fp;
    int sockInServer;

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        if ((valread = read(sockfd, buffer, 1024)) == 0)
        {
            perror("disconnect to server");
            exit(0);
        }
        else
        {
            pthread_mutex_lock(&mfile);
            if (buffer[0] == CODE_VALUE)
            {
                char val[4];
                memcpy(val, buffer + 1, 4);
                balls[itr++] = decode(val);
                printf("ball of sock %d: %d\n", sockInServer, balls[itr - 1]);

                qsort(balls, itr, sizeof(int), compare);
                
                fp = fopen(filename, "wb");
                fwrite(&itr, sizeof(int), 1, fp);
                fwrite(balls, itr * sizeof(int), 1, fp);
                fclose(fp);
            }
            else if (buffer[0] == CODE_OUT_OF_STOCK) {
                pthread_mutex_lock(&mutex);
                run_auto = 0;
                pthread_mutex_unlock(&mutex);
            }
            else if (buffer[0] == CODE_FILE)
            {
                // printf("CODE FILE %d\n", sockInServer);
                fp = fopen(filename, "rb");

                fseek(fp, 0, SEEK_END);
                int fsize = (int)ftell(fp);

                encode(sockInServer, buffer + 1);
                encode(fsize, buffer + 5);

                fseek(fp, 0, SEEK_SET);
                fread(buffer + 9, fsize, 1, fp);

                fclose(fp);

                write(sockfd, buffer, 1024);
            }
            else if (buffer[0] == CODE_SOCKET)
            {
                sockInServer = decode(buffer + 1);
                sprintf(filename, "client-src/%d", sockInServer);

                fp = fopen(filename, "wb");
                fwrite(&sockInServer, sizeof(int), 1, fp);
                fwrite(&itr, sizeof(int), 1, fp);
                fclose(fp);
            }
            else if (buffer[0] == CODE_RESULT) {
                char resultName[50];
                sprintf(resultName, "result-%d.txt", sockInServer);
                fp = fopen(resultName, "w");
                buffer[valread] = '\0';
                fprintf(fp, "%s", buffer + 1);
                fclose(fp);
            }
            else
            {
                buffer[valread] = '\0';
                printf("message of sock %d: ", sockInServer);
                puts(buffer);
            }
            pthread_mutex_unlock(&mfile);
        }
    }

    close(sockfd);
    free(balls);
}