#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <stdbool.h> 
#include <time.h>
#include <string.h>
#include <unistd.h>

#define WDIR 0
#define WSIZE 1
#define WOWNER 2
#define WGROUP 3 
#define WMONTH 4
#define WDAY 5

struct FILE_INFO
{
    char permisson[11];
    char filename[100];
    char owner[50];
    char group[50];
    char createTime[50];
    int size;
    int dir;
    int type;
    int month;
    int day;
};

bool read_directory(const char * name, char v[100][100], int * n);
void permisson(char modeval[], mode_t perm);
void printFormat(const struct FILE_INFO *files, int n, int w[]);
int countNumber(int number);
int countDir(const char * name);
void getMaxWidth(int w[], int t[], int n);

int main(int argc, char const *argv[])
{
    char v[100][100];
    int n = 0;
    int total = 0;
    char name[100];
    //Check if there is not input arguments, the default path is "."
    strcpy(name, argc == 2 ? argv[1] : ".");

    if(!read_directory(name, v, &n)) {
        printf("ls: cannot accsess '%s': No such file or directory\n", name);
        return -1;
    }

    strcat(name, "/");


    int w[6] = {0, 0, 0, 0, 2, 2};

    struct FILE_INFO files[100];

    for (int i = 0; i < n; i++)
    {
        struct stat st;
        char dirname[100];

        strcpy(dirname, name);
        strcat(dirname, v[i]);
        lstat(dirname, &st);
        char modeval[11];
        mode_t perm = st.st_mode;
  

        //permisson -rw-rw-r--
        permisson(modeval, perm);

        //owner
        struct passwd *pw = getpwuid(st.st_uid);
        //group
        struct group *gr = getgrgid(st.st_gid);

        int dir = S_ISDIR(st.st_mode) ? countDir(dirname) : st.st_nlink;
        files[i].type = S_ISREG(st.st_mode) ? 0 : 1;

        //format time
        char buffer[80];
        struct tm * my_tm = localtime(&st.st_ctime);
        strftime(buffer, sizeof(buffer), "%H:%M", my_tm);

        //total of file
        total += st.st_blocks;

        strcpy(files[i].filename, v[i]);

        //symbolic link
        if(S_ISLNK(st.st_mode)) {
            strcat(files[i].filename, " -> ");
            readlink(dirname, files[i].filename + strlen(files[i].filename), 100 - strlen(files[i].filename));
        }

        strcpy(files[i].permisson, modeval);
        files[i].size = st.st_size;
        files[i].dir = dir;

        strcpy(files[i].owner, pw->pw_name);
        strcpy(files[i].group, gr->gr_name);
        strcpy(files[i].createTime, buffer);

        files[i].month = my_tm->tm_mon + 1;
        files[i].day = my_tm->tm_mday;


        int t[6] = {
            countNumber(dir),
            countNumber(st.st_size),
            (int)strlen(files[i].owner),
            (int)strlen(files[i].group),
            countNumber(files[i].month),
            countNumber(files[i].day)};

        getMaxWidth(w, t, 6);
    }

    printf("total: %d\n", total * 512 / 1024);
    printFormat(files, n, w);

    return 0;
}

bool isHiddenFile(const char * name) {
    char * ret = strchr(name, '.');
    if(ret - name == 0) {
        return true;
    }
    return false;
}

bool read_directory(const char * name, char v[100][100], int * n)
{
    DIR *dirp = opendir(name);
    if(dirp == NULL)
        return false;

    *n = 0;
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        if (!isHiddenFile(dp->d_name)) {
            strcpy(v[*n], dp->d_name);
            *n = *n + 1;
        }
    }
    closedir(dirp);
    return true;
}

void permisson(char modeval[], mode_t perm)
{
    modeval[0] = (S_ISDIR(perm)) ? 'd' : '-';
    modeval[1] = (perm & S_IRUSR) ? 'r' : '-';
    modeval[2] = (perm & S_IWUSR) ? 'w' : '-';
    modeval[3] = (perm & S_IXUSR) ? 'x' : '-';
    modeval[4] = (perm & S_IRGRP) ? 'r' : '-';
    modeval[5] = (perm & S_IWGRP) ? 'w' : '-';
    modeval[6] = (perm & S_IXGRP) ? 'x' : '-';
    modeval[7] = (perm & S_IROTH) ? 'r' : '-';
    modeval[8] = (perm & S_IWOTH) ? 'w' : '-';
    modeval[9] = (perm & S_IXOTH) ? 'x' : '-';
    modeval[10] = '\0';
}

void printFormat(const struct FILE_INFO *files, int n, int w[])
{
    for (int i = 0; i < n; i++)
    {
        char color[3];
        strcpy(color, files[i].type == 0 ? "35" : "34");

        printf("%s%*d%*s%*s%*d Thg%*d%*d %s \033[1;%sm %s \033[0m\n",
         files[i].permisson,
         w[WDIR], files[i].dir,
         w[WOWNER], files[i].owner,
         w[WGROUP], files[i].group,
         w[WSIZE], files[i].size,
         w[WMONTH], files[i].month,
         w[WDAY], files[i].day,
         files[i].createTime,
         color,
         files[i].filename);
    }
}

int countNumber(int number)
{
    int count = 0;
    while (number != 0)
    {
        number /= 10;
        count++;
    }
    return count;
}

int countDir(const char * name)
{
    DIR *dirp = opendir(name);
    struct dirent *dp;
    int count = 0;
    while ((dp = readdir(dirp)) != NULL)
    {
        char filename[100];
        strcpy(filename, name);
        strcat(filename, "/");
        strcat(filename, dp->d_name);

        struct stat st;
        stat(filename, &st);
        if (S_ISDIR(st.st_mode))
        {
            count++;
        }
    }
    closedir(dirp);
    return count;
}

void getMaxWidth(int w[], int t[], int n)
{
    for (int i = 0; i < 6; i++)
    {
        if (t[i] > w[i] - 1)
            w[i] = t[i] + 1;
    }
}