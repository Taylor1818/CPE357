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

int childAvail(Child *children)
{
    for (int i = 1; i <= 10; i++)
    {
        if (children[i].pid == -1)
        {
            return i;
        }
    }
    return -1;
}

void findInner(Child *child, char *string)
{
}



void findFileName(Child *child, char *string)
{
    char result[1024];

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')// check stat and if needing to be recursive
            {
                if(strcmp(dir->d_name, string) == 0)
                {
                    strcpy(result, dir-> d_name);
                    fprintf(stderr, "%s", result);
                    //strcat(result, '\0');
                    //printf("%s%s\033[0m  ", dir->d_type != DT_DIR ? "\033[0m" : "\033[1;34m", dir->d_name);   
                }
                
            }
        }
        closedir(d);
    }
    //write(child->fd[1], result, 1024);
    printf("\n");
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

        for (int i = 0; i < 4; i++)
        {
            if (spaces != NULL)
            {
                strings[i] = spaces;
                spaces = strtok(NULL, " ");
                strings[i][strcspn(strings[i], "\n")] = '\0';
            }
        }

        if (strcmp("q", strings[0]) == 0 || strcmp("quit", strings[0]) == 0)
        {
            fprintf(stderr, "Quitting\n");
            quitting(children);
            break;
        }
        else if (strcmp("list", strings[0]) == 0)
        {
            list(children);
        }
        else if (strcmp("kill", strings[0]) == 0)
        {
            for (int i = 1; i <= 10; i++)
            {
                char temp[2];
                sprintf(temp, "%d", i); // int to string

                if (strcmp(strings[1], temp) == 0)
                {
                    killChild(children[i].pid);
                }
            }
        }
        else if (strcmp("find", strings[0]) == 0)
        {

            int cA = childAvail(children);

            char text[1024];

            strcpy(text, strings[1]);

            if (text[0] == '"')
            {
                int stringLength;
                for (int i = 1; text[i] != '\0'; i++)
                {
                    if (text[i] == '"')
                    {
                        stringLength = i - 1;
                        break;
                    }
                }
                char *toSearchFor = text + 1;
                stringLength[toSearchFor] = '\0';

                int pid = fork();
                if (pid == 0)
                {
                    findInner(children + cA, toSearchFor);
                }
            }
            else //No quotations
            {
                int pid = fork();
                if (pid == 0)
                {
                    (children+cA)->pid = getpid();
                    findFileName(children + cA, text);

                    char pipeRead[1024];
                    //read((children + cA)->fd[0], pipeRead, 1024 );

                    printf("%s", pipeRead);
                }
            }
        }
    }
}