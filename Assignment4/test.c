
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

// globals
int fd[2];
int found = 0;

void overwrite(int i)
{
    dup2(fd[0], STDIN_FILENO);
}

typedef struct arguments
{
    char command[100], target[100], flag1[100], flag2[100];
    int arg_count;
} arguments;

// for testing purposes
void print_struct(arguments* arg_ptr)
{
    printf("arg count: %d\n", arg_ptr->arg_count);
    printf("arg1: %s (length: %ld)\n", arg_ptr->command, strlen(arg_ptr->command));
    printf("arg2: %s (length: %ld)\n", arg_ptr->target, strlen(arg_ptr->target));
    printf("arg3: %s (length: %ld)\n", arg_ptr->flag1, strlen(arg_ptr->flag1));
    printf("arg4: %s (length: %ld)\n", arg_ptr->flag2, strlen(arg_ptr->flag2));
}

// helper function to parse the command line arguments, I DO NOT LOVE THIS FUNCTION
void extract_args(char* cmd, arguments* arg_ptr)
{
    // memset used to clear anything from the struct
    memset(arg_ptr->command, 0, 100);
    memset(arg_ptr->target, 0, 100);
    memset(arg_ptr->flag1, 0, 100);
    memset(arg_ptr->flag2, 0, 100);

    int count = 0;
    int start = 0;
    for (int i = 0; i <= strlen(cmd); i++)
    {
        // ending point is found
        if (cmd[i] == ' ' || cmd[i] == 0)
        {
            count++;
            // set the command
            if (count == 1)
            {
                strncpy(arg_ptr->command, cmd, i - start);
            }
            // set the target value (file, text, or PID)
            else if (count == 2)
            {
                strncpy(arg_ptr->target, cmd + start, i - start);
            }
            // set flag 1
            else if (count == 3)
            {
                strncpy(arg_ptr->flag1, cmd + start, i - start);
            }
            // set flag 2
            else if (count == 4)
            {
                strncpy(arg_ptr->flag2, cmd + start, i - start);
            }
            
            // update start index
            start = i + 1;
        }
    }
    // assign the number of args
    arg_ptr->arg_count = count;
}

// recursive search through subdirectories, this does not really work...
int search_sub(DIR *dir, struct dirent* dp, char* target, char cwd[1000], char* report)
{  
    while ((dp = readdir(dir)) != NULL)
    {
        //printf("%s\n", dp->d_name);
        //printf("[%s]\n", dp->d_name);
        // if file, check that it matches the target
        if(strcmp(dp->d_name, target) == 0)
        {
            found = 1;
            strcat(report, getcwd(cwd, 1000));
            strcat(cwd, '\0');
            return found;
        }    
        // directory, recurse
        else if((dp->d_type == DT_DIR) && (strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
        {
            if(opendir(dp->d_name) != NULL)
            {
                cwd = getcwd(cwd, 1000);
                strcat(cwd, "/");
                strcat(cwd, dp->d_name);
                strcat(cwd, '\0');

                //printf("%s\n", dp->d_name);
                //printf("%s\n", cwd);
                search_sub((opendir(dp->d_name)), readdir(opendir(dp->d_name)), target, cwd, report);
            }
        }   
    }
    return found;
}

int search(arguments* arg_ptr, char* report)
{   
    //int found = 0;
    int fileSearch = 1;
    int textSearch = 0;
    int sFlag = 0;
    int fFlag = 0;
    char fileEnding[50];
    memset(fileEnding, 0, 50);

    // check the target to see if file search or text search 
    if (strncmp("\"", arg_ptr->target, 1) == 0)
    {
        textSearch = 1;
        fileSearch = 0;
    }
    // check -s flag 
    if (strncmp("-s", arg_ptr->flag1, 2) == 0 || strncmp("-s", arg_ptr->flag2, 2) == 0)
    {
        sFlag = 1;
    }
    // check -f flag
    if (strncmp("-f:", arg_ptr->flag1, 3) == 0 || strncmp("-f:", arg_ptr->flag2, 3) == 0)
    {
        fFlag = 1;
        if (strncmp("-f:", arg_ptr->flag1, 3) == 0)
        {
            strncpy(fileEnding, (arg_ptr->flag1) + 3, strlen(arg_ptr->flag1) - 3);
        } 
        else
        {
            strncpy(fileEnding, (arg_ptr->flag2) + 3, strlen(arg_ptr->flag2) - 3);
        }
    }

    char cwd[1000];
    DIR *dir = opendir(getcwd(cwd, 1000)); // open current directory
    struct dirent *dp;
    //printf("%s\n", cwd);
    if (fileSearch == 1)
    {
        //printf("---file search time---\n");
        // current directory + subdirectories 
        if (sFlag)
        {
            //printf("---time to check the sub---\n");
            search_sub(dir, dp, arg_ptr->target, cwd, report);
            //printf("%d\n", found);
            //printf("%s\n", cwd);
            //printf("%s\n", report);
        }
        // current directory
        else
        {
            while((dp = readdir(dir)) != NULL)
            {
                if (strcmp(dp->d_name, arg_ptr->target) == 0)
                {
                    found = 1;
                    strcat(report, getcwd(cwd, 1000));
                }
            }
        }
    }
    else if (textSearch == 1)
    {
        char text[100];
        char* target = arg_ptr->target;
        for (int i = 0; i < strlen(target); i++)
        {
            if (target + i != "\"")
            {
                strncpy(text, target + i, 1);
            }
        }
        strcat(text, '\0');
        printf("%s\n", text);

        //printf("---text search time---\n");
        // search the current directory
        while((dp = readdir(dir)) != NULL)
        {
            struct stat st;
            char* path = cwd;
            stat(dp->d_name, &st);
            strcat(path, "/");
            strcat(path, dp->d_name);

            FILE *file = fopen(dp->d_name, "rb"); 
            if(file == NULL)
            {
                fclose(file);
                continue;
            }
            char *data = malloc(st.st_size);
            fread(data, 1, st.st_size, file);
            fclose(file);

            if(strstr(data, text) != 0)
            {
                found = 1;
                strcat(report, path);
            }
        }   
    }

    //close(dir);
    return found;
}

int main()
{
    // parent PID
    int* parentPID = (int*)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *parentPID = getpid();
    // keep track of all child PIDs (up to 10 children)
    int* childPIDs = (int*)mmap(NULL, sizeof(int) * 10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // prompt user for a command
    printf("Enter commmand:\n");
    printf("[find <file>]           tries to find file in current directory\n");
    printf("[find <file> -s]        tries to find file in current directory/subdirectory\n");
    printf("[find <\"text\">]         searches for text in all files of current directory\n");
    printf("[find <\"text\"> -f:X]    searches for text in all files of current directory with file ending\n");
    printf("[find <\"text\"> -f:X -s] searches for text in all files of current directory/subdirectory with file ending\n");
    printf("[list]                  displays all running child processes and their serial number\n");
    printf("[kill <PID>]            kills a child process\n");
    printf("[quit] or [q]           quits the program and all child processes\n");
    
    // buffer for command line
    char cmd[1000];

    signal(SIGUSR1, overwrite);
    int saveSTDIN = dup(STDIN_FILENO);
    pipe(fd);

    for(;;)
    {
        printf("\n\033[0;34m"); // set to blue
        printf("findstuff");
        printf("\033[0m"); // reset color
        printf("$ ");
        fflush(0);

        // read and add null character
        int r = read(STDIN_FILENO, cmd, 1000);
        cmd[r - 1] = 0;

        // restore STDIN
        dup2(saveSTDIN, STDIN_FILENO);

        // quit or q, end program
        if (strcmp("quit", cmd) == 0 || strcmp("q", cmd) == 0)
        {
            for (int i = 0; i < 10; i++)
            {
                if (childPIDs[i] != 0)
                {
                    kill(childPIDs[i], SIGKILL);
                    childPIDs[i] = 0;
                }
            }

            kill(*parentPID, SIGKILL);
            wait(0);
            break;
        }
        // kill <PID>, compare first 4 characters
        else if (strncmp("kill", cmd, 4) == 0)
        {
            // extract the arguments
            arguments* arg_ptr;
            arguments args;
            arg_ptr = &args;
            extract_args(cmd, arg_ptr);
            // set the PID of the child to be killed
            int kill_child = atoi(arg_ptr->target);

            // search for specific child
            for (int i = 0; i < 10; i++)
            {
                // look for child pid
                if (childPIDs[i] == kill_child)
                {
                    kill(kill_child, SIGKILL);
                    waitpid(kill_child, 0, 0);
                    childPIDs[i] = 0;
                    break;
                }
            }
        }
        // list all children (0-9)
        else if (strcmp("list", cmd) == 0)
        {
            char notRunning[] = "not running";
            char searching[] = "searching";
            for (int i = 0; i < 10; i++)
            {
                // child is alive
                if (childPIDs[i] != 0)
                {
                    printf("Child #%d: PID %d [status: %s]\n", i, childPIDs[i], searching); 
                }
                // child is not running
                else
                {
                    printf("Child #%d: PID %d [status = %s]\n", i, childPIDs[i], notRunning); 

                }
            }
        }
        // time to find!!
        else if (strncmp("find", cmd, 4) == 0)
        {
            // continue on and fork
            if (fork() == 0)
            {
                int childIndex = 0;
                // try to find a spot for the child
                for (int i = 0; i < 10; i++)
                {
                    if (childPIDs[i] == 0)
                    {
                        childPIDs[i] = getpid();
                        //printf("%d\n", childPIDs[i]);
                        childIndex = i;
                        break;
                    }
                }

                clock_t start;
                start = clock();

                // intialize arguments pointer and struct, then parse for args
                arguments* arg_ptr;
                arguments args;
                arg_ptr = &args;
                extract_args(cmd, arg_ptr);

                //int found = 0;
                char report[10000];
                search(arg_ptr, report);

                clock_t stop;
                stop = clock();
                double time = ((double)(stop-start));
                double seconds = time/CLOCKS_PER_SEC;
                char sec[100];
                sprintf(sec, "%f", seconds);
                strcat(report, "\nTime: ");
                strcat(report, sec);
                strcat(report, " seconds");

                if (found == 1)
                {
                    //printf("yay i found the file!\n");
                    //printf("%s\n", report);
                    strcat(report, "\nInput was found.\n");
                }
                else
                {
                    //printf("i did not find the file...\n");
                    strcat(report, "\nInput was not found.\n");
                }

                found = 0;
                close(fd[0]);
                write(STDOUT_FILENO, report, strlen(report));
                close(fd[1]);

                kill(*parentPID, SIGUSR1);
                childPIDs[childIndex] = 0;

                return 0;
            }
        }
        
        for (int i = 0; i < 10; i++)
        {
            if (childPIDs[i] != 0)
            {
                waitpid(childPIDs[i], 0, WNOHANG);
            }
        }
    }

    munmap(childPIDs, sizeof(int) * 10);
    munmap(parentPID, 4);

    return 0;
}