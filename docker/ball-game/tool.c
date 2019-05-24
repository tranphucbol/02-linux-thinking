#include "tool.h"

void encode(int n, unsigned char buffer[])
{
    for (int i = 0; i < 4; i++)
    {
        unsigned char tmp = ((n >> (i * 8)) & 0xff);
        buffer[i] = tmp;
    }
}

int decode(unsigned char buffer[])
{
    int n = 0;
    for (int i = 0; i < 4; i++)
    {
        n = (buffer[i] << (i * 8)) | n;
    }
    return n;
}

void getFile(char buffer[], const char * parent) {
    char filename[50];
    sprintf(filename, "%s%d", parent, decode(buffer));
    FILE * fp = fopen(filename, "wb");
    fwrite(buffer + 8, decode(buffer + 4), 1, fp);
    fclose(fp);
}
