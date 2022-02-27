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
#include <sys/times.h>
#include <fcntl.h>

/*
    Exercise: Two-Way Pipes
    a. Parent sends message to child ("Hi Kid") and
       Child sends message to parent ("Hi Mom & Dad").
    Use pipes
*/

/*
    redirect with dup and dup2
*/

void main()
{
    // int pd[2];
    // int cd[2];
    // pipe(cd);
    // pipe(pd);

    // int pid = fork();
    // if(pid == 0){

    //     write(cd[1], "Hi Mom & Dad\0", 100);

    //     char ptext[100];
    //     read(pd[0], ptext, 100);
    //     printf("%s", ptext);
    // }
    // else{

    //     write(pd[1], "Hi Kid\0", 100);

    //     char ctext[100];
    //     read(cd[0], ctext, 100);
    //     printf("%s", ctext);

    // }

    //REDIRECTION
    // int fd[2];
    // pipe(fd);

    // int pid = fork();
    // if (pid == 0)
    // {
    //     sleep(2);
    //     write(fd[1], "Hi Mom & Dad\0", 100);
    //     dup2(STDIN_FILENO, fd[0]);
    //     kill(getppid(), SIGUSR1);
    //     close(fd[1]);
    // }
    // else
    // {
    //     char ptext[100];
    //     // int savedOut = dup(STDIN_FILENO);
    //     // dup2(cd[1], STDIN_FILENO);

    //     // read(savedOut, ptext, 100);
    //     while (1)
    //     {
    //         read(STDIN_FILENO, ptext, 100);
    //         printf("%s", ptext);
    //     }


        // char ctext[100];
        // read(cd[0], ctext, 100);
        // printf("%s", ctext);
    //}
}