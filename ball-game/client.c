#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <pthread.h>

#define PORT 8080
#define MAXLINE 1024

#define CODE_VALUE 1

void *inputThread(void *sockfd)
{
    char buffer[MAXLINE];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        // fgets(buffer, 1024, stdin);
        scanf("%[^\n]%*c", buffer); 
        write(*(int *)sockfd, buffer, sizeof(buffer));
    }
}

int decode(char buffer[]);

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;
    int valread;

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
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, inputThread, &sockfd);

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
            if (buffer[0] == CODE_VALUE)
            {
                char val[8];
                memcpy(val, buffer + 1, 8);
                int n_val = decode(val);
                printf("Message from server: %d\n", n_val);
            }
            else
            {
                buffer[valread] = '\0';
                printf("Message from server: ");
                puts(buffer);
            }
        }
    }

    pthread_join(thread_id, NULL);
    close(sockfd);
}

int decode(char buffer[]) {
    int n = 0;
    for(int i=0; i < 8; i++) {
        n = (buffer[i] << (i * 4)) | n;
    }
    return n;
}