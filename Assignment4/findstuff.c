#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define START_DIR = "."

typedef struct Child
{
    int pid;
    int fd[2];
    int status;
} Child;

void childProcess()
{
}

int find()
{
}

int findSubDir()
{
}

void quitting(Child *children)
{
    for (int i = 0; i < 10; i++)
    {
        if (children[i].pid > 0)
        {
            kill(children[i].pid, SIGKILL);
            children[i].status = 0;
        }
    }
    wait(0);
}

void killChild(int child)
{
    printf("child killed\n");
    // kill(child, SIGKILL);
}

void list(Child *children)
{
    char notRunning[] = "Not Running";
    char running[] = "Running";

    for (int i = 1; i <= 10; i++)
    {
        if (children[i].status == 0) // not running
        {
            printf("Child %d: PID: %d Status: %s\n", i, children[i].pid, notRunning);
        }
        else // running
        {
            printf("Child %d: PID: %d Status: %s\n", i, children[i].pid, running);
        }
    }
}

void main()
{
    Child *children = (Child *)mmap(0, sizeof(Child) * 10, 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    for (int i = 1; i <= 10; i++)
    {
        children[i].pid = -1;
        children[i].status = 0;
    }

    char userInput[1024];

    while (1)
    {
        fprintf(stderr, "\033[1;34mfindstuff$\033[0m$ ");
        fgets(userInput, sizeof(userInput), stdin);

        // Gets all strings and store them
        char *spaces = strtok(userInput, " ");
        char *strings[4];

        while (spaces != NULL)
        {
            for (int i = 0; i < 4; i++)
            {
                strings[i] = spaces;
                spaces = strtok(NULL, " ");
            }
        }

        if (strcmp("q\n", strings[0]) == 0 || strcmp("quit\n", strings[0]) == 0)
        {
            fprintf(stderr, "Quitting\n");
            quitting(children);
            break;
        }
        else if (strcmp("list\n", strings[0]) == 0)
        {
            list(children);
        }
        else if (strcmp("kill", strings[0]) == 0)
        {
            for (int i = 1; i <= 10; i++)
            {
                char temp[2];
                sprintf(temp, "%d", i); // int to string
                strcat(temp, "\n");     // add ending

                if (strcmp(strings[1], temp) == 0)
                {
                    killChild(children[i].pid);
                }
            }
        }
        else if (strcmp("find", strings[0]) == 0)
        {
            char text[100] = &strings[1];
            //text = *strings[1];
            if (strcmp(strings[1], "text\n") == 0)
            {
                printf("here");
            }
        }
    }
}