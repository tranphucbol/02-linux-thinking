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
int compare(const void *a, const void *b);

int main(int argc, char const *argv[])
{
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr;
    int valread;
    int itr = 0;
    int *balls = (int *)malloc(1000 * sizeof(int));

    srand((unsigned int)time(0));

    if (argc >= 2)
    {
        run_auto = 1;
    }

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

    memset(buffer, 0, 1024);
    valread = read(sockfd, buffer, 1024);

    int sockInServer = atoi(buffer + 1);
    char filename[50];
    sprintf(filename, "client-src/%d", sockInServer);

    FILE *fp = fopen(filename, "wb");
    fwrite(&sockInServer, sizeof(int), 1, fp);
    fwrite(&itr, sizeof(int), 1, fp);
    fclose(fp);

    while (TRUE)
    {
        memset(buffer, 0, 1024);
        if(run_auto == 1) {
            buffer[0] = CODE_GET;
            send(sockfd, buffer, 1024, 0);
        } else {
            scanf("%[^\n]%*c", buffer);
            if(strcmp(buffer, "get") == 0) {
                buffer[0] = CODE_GET;
                send(sockfd, buffer, 1024, 0);
            } else
                continue;
        }

        valread = read(sockfd, buffer, 1024);

        if (buffer[0] == CODE_OUT_OF_STOCK)
        {
            printf("out of stock\n");
            break;
        }
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

    memset(buffer, 0, 1024);
    valread = read(sockfd, buffer, 1024);
    buffer[0] = CODE_FILE;

    fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    int fsize = (int)ftell(fp);

    encode(sockInServer, buffer + 1);
    encode(fsize, buffer + 5);

    fseek(fp, 0, SEEK_SET);
    fread(buffer + 9, fsize, 1, fp);

    fclose(fp);
    send(sockfd, buffer, 1024, 0);

    memset(buffer, 0, 1024);
    valread = read(sockfd, buffer, 1024);

    char resultName[50];
    sprintf(resultName, "result-%d.txt", sockInServer);
    printf("%s\n", resultName);
    fp = fopen(resultName, "w");
    buffer[valread] = '\0';
    fprintf(fp, "%s", buffer + 1);
    fclose(fp);

    close(sockfd);

    return 0;
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}