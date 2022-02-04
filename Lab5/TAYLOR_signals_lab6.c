#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>

void onkill()
{
    printf("CANT KILL ME!");
}

void childTask(int parentid)
{
    printf("Parent ID: %d\n", parentid);
    printf("Child ID: %d\n", getpid());

    printf("Files in current directory: \n");

    DIR *d;

    struct dirent *dir;
    d = opendir(".");

    if(d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(dir -> d_name[0] != '.')
            {
                printf("[%s] \n", dir -> d_name);
            }
        }
        
    }
    printf("\n");
}

void nokill()
{
    signal(SIGINT, onkill);
    signal(SIGTERM, onkill);
    signal(SIGQUIT, onkill);
    signal(SIGHUP, onkill);
    signal(SIGTSTP, onkill);
    signal(SIGSTOP, onkill);
}

void main()
{
    int pid;

    while(1)
    {
        pid = fork();

        while ( pid == 0 )
        {
            nokill();

            time_t T = time(NULL);
            struct tm *now = localtime(&T);

            printf ("\nTime: %02d: %02d: %02d\n", now -> tm_hour, now -> tm_min, now -> tm_sec);

            childTask( getppid() );
            sleep(10); 
        }

        if(pid > 0)
        {     
            nokill();
            wait(0);
        }
    }
    return;
}