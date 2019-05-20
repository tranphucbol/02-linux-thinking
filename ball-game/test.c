#include <stdio.h>
#define MAX 1024
int main()
{
    // char buf[MAX];
    // scanf("%[^\n]%*c", buf);
    FILE *fp;
    fp = fopen("3", "rb");
    int n = 8;
    int arr[8];
    fread(arr, 8 * sizeof(int), 1, fp);
    for(int i=0; i<8; i++) {
        printf("%d\n", arr[i]);
    }
    fclose(fp);
    return 0;
}