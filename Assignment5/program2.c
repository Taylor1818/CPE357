#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Incorrect number of arguments!\n");
        return 0;
    }

    char *args[4];
    args[0] = malloc(100);
    args[1] = malloc(100);
    args[2] = malloc(100);
    args[3] = NULL;

    strcpy(args[0], argv[1]);
    strcpy(args[2], argv[2]);

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        if (fork() == 0)
        {
            sprintf(args[1], "%d", i);
            execv("program1", args);
        }
    }
    waitpid(getppid(), NULL, 0);
    return 0;
}