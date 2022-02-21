#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void main()
{

    enum flagState
    {
        IDLE,
        WAITING,
        ACTIVE
    };

    enum flagState *flags = (enum flagState *)mmap(0, sizeof(enum flagState) * 2, 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    char *text = (char *)mmap(0, 1000, 0x1 | 0x2, 0x20 | 0x01, -1, 0);
    int *turn = (int *)mmap(0, sizeof(int), 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    *turn = 0;
    flags[0] = IDLE;
    flags[1] = IDLE;

    int pid = fork();

    if (pid == 0)
    {
        int i = 0;
        int process = 0;
        int count = 0;
        char *string1 = "Don't be trapped by dogma which is living with the results of other people's thinking.";
        char *string2 = "Many of life's failures are people who did not realize how close they were to success when they gave up.";

        while (1)
        {
            while (1)
            {
                flags[i] = WAITING;

                process = *turn;

                while (process != i)
                {
                    if (flags[process] != IDLE)
                    {
                        process = *turn;
                    }
                    else
                    {
                        process = (process + 1) % 2;
                    }
                }

                flags[i] = ACTIVE;

                process = 0;

                while ( (process < 2) && ( (process == i) || (flags[process] != ACTIVE) ) )
                {
                    process = process + 1;
                }

                if ((process >= 2) && ((*turn == i) || flags[*turn] == IDLE))
                {
                    break;
                }

            } 

            *turn = i;

            //critical section
            if (count % 2 == 0)
            {
                strcpy(text, string1);
            }
            else
            {
                strcpy(text, string2);
            }
            count++;
            //end of critical section

            
            process = (*turn + 1) % 2;
            while (flags[process] == IDLE)
            {
                process = (process + 1) % 2;
            }

            *turn = process;

            flags[i] = IDLE;
        }
    }
    else if (pid > 0)
    {
        int i = 1;
        int process = 0;

        while (1)
        {
            while(1)
            {
                flags[i] = WAITING;

                process = *turn;

                while (process != i)
                {
                    if (flags[process] != IDLE)
                    {
                        process = *turn;
                    }
                    else
                    {
                        process = (process + 1) % 2;
                    }
                }

                flags[i] = ACTIVE;

                process = 0;
                while ((process < 2) && ((process == i) || (flags[process] != ACTIVE)))
                {
                    process = process + 1;
                }

                if ((process >= 2) && ((*turn == i) || flags[*turn] == IDLE))
                {
                    break;
                }
                
            } 

            *turn = i;

            //critical section
            char print[1000];
            strcpy(print, text);
            printf("%s\n", print);
            //end of critical section

            process = (*turn + 1) % 2;
            while (flags[process] == IDLE)
            {
                process = (process + 1) % 2;
            }

            *turn = process;

            flags[i] = IDLE;

        }

        munmap(text, 1000);
        munmap(turn, sizeof(int));
        munmap(flags, sizeof(enum flagState));
    }
}