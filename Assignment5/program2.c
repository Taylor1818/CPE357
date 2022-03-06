#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    // COMMAND LINE = ./program2 program1 N
    if (argc != 3)
    {
        printf("Incorrect number of arguments!\n");
        return 0;
    }

    char* args[4];
    args[0] = malloc(100);
    args[1] = malloc(100);
    args[2] = malloc(100);
    args[3] =  NULL;

    // "./program1", "i", "N", NULL
    strcpy(args[0], argv[1]); // program 1
    strcpy(args[2], argv[2]); // N

    // number of processes as an int

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        if (fork() == 0)
        {
            // get the process ID
            sprintf(args[1], "%d", i);
            // call program 1
            execv("program1", args);
            return 0;
        }
    }

    wait(0);
    return 0;
}