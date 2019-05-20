#include <stdio.h>
#define MAX 1024
int main()
{
    char buf[MAX];
    scanf("%[^\n]%*c", buf);
    FILE *fp;
    fp = fopen("test.txt", "a");
    fprintf(fp, "ahihi\n");
    fclose(fp);
    return 0;
}