#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void list(char *location)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(location);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')
            {
                printf("%s%s\033[0m  ", dir->d_type != DT_DIR ? "\033[0m" : "\033[1;34m", dir->d_name);
            }
        }
        closedir(d);
    }
    printf("\n");
}

void onkill()
{
}

void nokill()
{
    signal(SIGINT, onkill);
    signal(SIGTERM, onkill);
    signal(SIGQUIT, onkill);
    signal(SIGHUP, onkill);
    signal(SIGTSTP, onkill);
    signal(SIGSTOP, onkill);
    signal(SIGRTMIN, onkill);
}

void childProcess()
{
    char flush;
    char text[1000];
    char *currentDir, *currentDirBuf;
    long size;

    while (1)
    {
        size = pathconf(".", _PC_PATH_MAX);

        if ((currentDirBuf = (char *)malloc((size_t)size)) != NULL)
        {
            currentDir = getcwd(currentDirBuf, (size_t)size);
        }

        printf("\033[0;34mstat prog . %s\033[0m$", currentDir);

        scanf("%[^\n]", text);
        scanf("%c", &flush);

        if (!strcmp(text, "list"))
        {
            list(".");
        }
        else if (!strcmp(text, ".."))
        {
            chdir("..");
        }
        else if (text[0] == '/')
        {
            char temp[1001] = ".";
            struct stat stats;

            strcat(temp, text);

            if (stat(temp, &stats) || S_ISREG(stats.st_mode))
            {
                printf("Invalid Directory\n");
            }
            else
            {
                chdir(temp);
            }
        }
        else if (!strcmp(text, "q"))
        {
            kill(getppid(), SIGKILL);
            kill(getpid(), SIGKILL);
        }
        else
        {
            printf("Invalid File\n");
        }
    }
}

void main()
{

    int pid;
    while (1)
    {
        pid = fork();

        if (!pid)
        {
            nokill();
            
            childProcess();
        }
        else
        {
            nokill();

            printf("Parent ID: %d   Child ID: %d \n", getpid(), pid);
            wait(0);
            sleep(10);
        }
    }
}