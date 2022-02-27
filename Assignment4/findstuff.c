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


void parentProcess()
{
}
void childProcess()
{
}

int find()
{
}

int findSubDir()
{
}

void quit(int *children, int *parent)
{
    for (int i = 0; i < 10; i++)
    {
        if (children[i] != 0)
        {
            kill(children[i], SIGKILL);
            children[i] = 0;
        }
        
    }
    kill(parent, SIGKILL);
    wait(0);
}

void killchild(int pid, int *childPid)
{
     for (int i = 0; i < 10; i++)
    {
        if (childPid[i] == pid)
        {
            kill(childPid[i], SIGKILL);
            childPid[i] = 0;
        }
    }
}

void list(int *children)
{
    char notRunning[] = "Not Running";
    char running[] = "Running";

    for (int i = 1; i <= 10; i++)
    {
        if (children[i] == 0) //not running
        {
            printf("Child %d: PID: %d Status: %s\n", i, children[i], notRunning);
        }
        else //running
        {
            printf("Child %d: PID: %d Status: %s\n", i, children[i], running);
        }
    }
}

void main()
{
    int *parentPid = (int *)mmap(NULL, sizeof(int), 0x1 | 0x2, 0x20 | 0x01, -1, 0);
    int *childPids = (int *)mmap(NULL, 10 * sizeof(int), 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    list(childPids);
}