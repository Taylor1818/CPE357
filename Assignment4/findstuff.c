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

//taylor

typedef struct Child
{
    int pid;
    int fd[2];
    int status;
} Child;

Child *children;

int childAvail()
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

int searchRecursive(char *string, Child *child, int quotes, int recursive, char *fileType, char *path, int *find)
{
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        return -1;
    }

    struct dirent *d;


    while ((d = readdir(dir)) != NULL)
    {

        if (d->d_name[0] == '.' && (d->d_name[1] == '\0' || d->d_name[1] == '.' && (d->d_name[2] == '\0')))
        {
            continue;
        }
        struct stat statBlock;
        char filePath[1024];

        strcpy(filePath, path);
        strcat(filePath, "/");
        strcat(filePath, d->d_name);
        lstat(filePath, &statBlock);

        if (d->d_type == 4) // is folder
        {

            if (recursive == 1)
            {

                searchRecursive(string, child, quotes, recursive, fileType, filePath, find);
            }
        }
        else // file
        {

            if (quotes == 0) // search just name
            {
                if (strcmp(d->d_name, string) == 0)
                {
                    *find = *find + 1;
                    write(child->fd[1], filePath, strlen(filePath));
                }
            }
            else // quotes exist check file contents
            {

                if (statBlock.st_blocks > 0)
                {

                    FILE *f = fopen(filePath, "r");

                    if (f == NULL)
                    {
                        break;
                    }

                    char *line;
                    size_t len = 0;
                    ssize_t read;

                    while ((read = getline(&line, &len, f)) != -1)
                    {
                        
                        
                        line[strcspn(line, "\n")] = '\0';
                        
                        if (strstr(line, string) != NULL )
                        {
                            
                            fclose(f);
                            free(line);
                            *find = *find + 1;
                            write(child->fd[1], filePath, strlen(filePath));
                            continue;
                        }
                    }

                    fclose(f);
                    free(line);
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

int search(char *string, Child *child, char *strings[], int quotes)
{

    // find text -s
    // find text -f:c
    // find text -f:c -s

    // find "text" -s
    // find "text" -f:c
    // find "text" -f:c -s || -s -f:c
    time_t startTime = clock();
    int recursive = 0;
    char *fileType = NULL;
    int found = 0;

    if (strings[2] != NULL)
    {
        if (strcmp(strings[2], "-s") == 0) // searchSub
        {
            recursive = 1;
        }
        if (strcmp(strings[2], "-f:") > 0)
        {
            fileType = strings[2] + 3;
        }
       
    }
    if (strings[3] != NULL)
    {
        if (strcmp(strings[3], "-s") == 0)
        {
            recursive = 1;
        }

        if (strcmp(strings[3], "-f:") > 0)
        {
            fileType = strings[3] + 3;
        }
    }

    searchRecursive(string, child, quotes, recursive, fileType, ".", &found);

    char message[100];
    int mils = (((double)(clock() - startTime)) / CLOCKS_PER_SEC * 1000);
    int hh = mils / 3600000;
    int mm = (mils % 3600000) / 60000;
    int ss = (mils % 60000) / 1000;
    int ms = (mils % 1000);

    snprintf(message, sizeof(message), "\nFound %d files in %02d:%02d:%02d:%02d\n", found, hh, mm, ss, ms);

    write(child->fd[1], message, 100);

    child->status = 1;
    kill(getppid(), SIGUSR1);

    close(child->fd[1]);
    exit(0);
}

int findSetup(Child *child, char *string, char *strings[], int quotes)
{
    pipe(child->fd);
    int pid = fork();

    if (pid == 0)
    {

        close(child->fd[0]);
        search(string, child, strings, quotes);
    }
    else if (pid > 0)
    {
        child->status = 1;
        child->pid = pid;
        close(child->fd[1]);
        return 0;
    }
    else
    {
        fprintf(stderr, "failed to fork child");
        return -1;
    }
}
void killChild(int child, int index)
{
    printf("child killed\n");
    kill(child, SIGKILL);
    children[index].pid = -1;
    children[index].fd[1] = 0;
    children[index].fd[0] = 0;
    children[index].status = 0;
}

void quitting()
{
    for (int i = 0; i < 10; i++)
    {
        if (children[i].pid > 0)
        {
            killChild(children[i].pid, i);
            children[i].status = 0;
        }
    }
    wait(0);
}



void list()
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

void handler(int i)
{
    int saveIn = dup(STDIN_FILENO);
    int c = -1;
    for (int i = 0; i < 10; i++)
    {
        if (children[i].status == 1)
        {
            children[i].status = 0;
            c = i;
            dup2(children[i].fd[0], STDIN_FILENO);

            char result;
            fprintf(stderr, "\n");

            while (read(STDIN_FILENO, &result, 1) && result != '\0')
            {
                write(STDERR_FILENO, &result, 1);
            }
        }
    }

    dup2(saveIn, STDIN_FILENO);
    waitpid(children[c].pid, NULL, 0);

    close(children[c].fd[0]);
    children[c].pid = -1;
    children[c].fd[1] = 0;
    children[c].fd[0] = 0;

    write(STDERR_FILENO, "Press ENTER to continue", 24);
}

void main()
{
    children = (Child *)mmap(0, sizeof(Child) * 10, 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    for (int i = 1; i <= 10; i++)
    {
        children[i].pid = -1;
        children[i].status = 0;
    }

    signal(SIGUSR1, handler);

    char userInput[1024];

    while (1)
    {
        fprintf(stderr, "\033[1;34mfindstuff$\033[0m ");
        fgets(userInput, 1024, stdin);

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
            else
            {
                strings[i] = NULL;
            }
        }

        if (strcmp("q", strings[0]) == 0 || strcmp("quit", strings[0]) == 0)
        {
            fprintf(stderr, "Quitting\n");
            quitting();
            break;
        }
        else if (strcmp("list", strings[0]) == 0)
        {
            list();
        }
        else if (strcmp("kill", strings[0]) == 0)
        {
            for (int i = 1; i <= 10; i++)
            {
                char temp[2];
                sprintf(temp, "%d", i); // int to string

                if (strcmp(strings[1], temp) == 0)
                {
                    killChild(children[i].pid, i);
                }
            }
        }
        else if (strcmp("find", strings[0]) == 0)
        {

            int cA = childAvail();

            char text[1024];
            if (strings[1] != NULL)
            {
                strcpy(text, strings[1]);
            }

            if (cA == -1)
            {
                printf("all children busy.");
            }
            else
            {
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

                    findSetup(children + cA, text, strings, 1);
                }
                else // No quotations
                {
                    findSetup(children + cA, text, strings, 0);
                }
            }
        }
    }
}
