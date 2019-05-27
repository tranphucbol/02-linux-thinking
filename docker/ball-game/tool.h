#ifndef TOOL
#define TOOL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define MAX_CLIENT 10

#define TRUE 1
#define FALSE 0

#define CODE_VALUE 1
#define CODE_FILE 2
#define CODE_SOCKET 3
#define CODE_RESULT 4
#define CODE_OUT_OF_STOCK 5
#define CODE_GET 6
#define CODE_MSG 7
#define CODE_START_FILE 8
#define CODE_END_FILE 9

void encode(int n, unsigned char buffer[]);
int decode(unsigned char buffer[]);
void getFile(char buffer[], const char * parent);

#endif //TOOL
