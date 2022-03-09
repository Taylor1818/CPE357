/*
Write a program countdown that starts any other program of your choice after a certain time has
passed. While waiting for the “launch” of the chosen program, output a countdown that shows one
period for every 0.5 seconds that are remaining before the launch. Hence, if the time to launch is 3
seconds, output
...... //3 seconds until START
..... //2.5 seconds until START
.... //2 seconds until START
... //1.5 seconds until START
.. //1 second until START
. //0.5 seconds until START
 // START of other program

How to:

You must use the system call setitimer to generate the SIGALRMs for each clock tick (Read the man
pages!).
Because the display requires half-second accuracy, the granularity of the timer must be finer than one tick
per second.
Remember that stdio is buffered. If you write something to the console with putchar() or printf(), it may
not appear immediately. You should either use unbuffered writes, or flush the output stream after writing.


I will not test unexpected input; hence you may assume the command-line is well formed.

*/

// int getitimer(ITIMER_REAL, itimerval);

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>

char *dots;

void handler(int i)
{
    fprintf(stderr, "%s", dots);
    dots++;
}

int main(int argc, char *argv[])
{
    // ./countdown 3 ./myotherprogram
    //          argv[0] argv[1]
    // ./countdown 4.5 ./programwithcommandlineargs hi 34

    // ...... //3 seconds until START
    // ..... //2.5 seconds until START
    // .... //2 seconds until START
    // ... //1.5 seconds until START
    // .. //1 second until START
    // . //0.5 seconds until START
    //  // START of other program

    int timer = atoi(argv[1]);  // gets the wait timer length before exec program
    char *executable = argv[2]; // file to execute

    dots = malloc(timer * 2 + 2);
    for (int i = 0; i < (timer * 2); i++)
    {
        dots[i] = '.';
    }

    dots[timer * 2] = '\n';
    dots[timer * 2 + 1] = '\0';

    char *newDots = dots;

    struct itimerval time;

    time.it_interval.tv_sec = 0;
    time.it_interval.tv_usec = 500000;
    time.it_value.tv_sec = 0;
    time.it_value.tv_usec = 500000;

    signal(SIGALRM, handler);

    setitimer(ITIMER_REAL, &time, NULL);

    while (newDots + timer *2 +1> dots);

    if (fork() == 0)
    {
        execv(executable, argv + 2);
    }
    wait(0);

    return 0;
}