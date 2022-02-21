#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


void main()
{
    char *text = (char *)mmap(0, 1000, 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    int *vars = (int *)mmap(0, 3 * sizeof(int), 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    int *turn = vars;
    int *wantsIn = &vars[1];

    *turn = 1;
    wantsIn[0] = 1;
    wantsIn[1] = 1;

    int pid = fork();

    if (pid == 0)
    {
        char *string1 = "Don't be trapped by dogma which is living with the results of other people's thinking.";
        char *string2 = "Many of life's failures are people who did not realize how close they were to success when they gave up.";
        int count = 0;
        while (1)
        {

            wantsIn[0] = 1;
            

            while (*turn == 1 && wantsIn[1] == 1)
            {
                //critical Section
                if (count % 2 == 0)
                {
                    strcpy(text, string1);
                }
                else
                {
                    strcpy(text, string2);
                } //end Crit Sect
                *turn = 0;
                wantsIn[0] = 0;
                count++;
            }
            
        }
    }
    else if (pid > 0)
    {

        while (1)
        {
            wantsIn[1] = 1;
            while (*turn == 0 && wantsIn[0] == 1)
            {
                //critical Section
                char print[1000];
                strcpy(print, text);
                printf("%s\n", print);
                //end Crit Sect
                *turn = 1;
                wantsIn[1] = 0;
            }
        }
    }

    munmap(text, 1000);
    munmap(vars, 3 * sizeof(int));
}