#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void main()
{

    enum pstate
    {
        IDLE,
        WAITING,
        ACTIVE
    };

    enum pstate *flags = (enum pstate *)mmap(0, sizeof(enum pstate) * 2, 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    char *text = (char *)mmap(0, 1000, 0x1 | 0x2, 0x20 | 0x01, -1, 0);
    int *turn = (int *)mmap(0, sizeof(int), 0x1 | 0x2, 0x20 | 0x01, -1, 0);

    *turn = 0;
    flags[0] = IDLE;
    flags[1] = IDLE;

    int pid = fork();
    
    if (pid == 0)
    {

        char *string1 = "Your time is limited, so don't waste it living someone else's life. Don't be trapped by dogma which is living with the results of other people's thinking.";
        char *string2 = "Many of life's failures are people who did not realize how close they were to success when they gave up.";

        while (1)
        {
            int i = 0;
            int index = 0;
            int count = 0;
            do
            {
                /* announce that we need the resource */
                flags[i] = WAITING;

                /* scan processes from the one with the turn up to ourselves. */
                /* repeat if necessary until the scan finds all processes idle */
                index = *turn;
                while (index != i)
                {
                    if (flags[index] != IDLE)
                    {
                        index = *turn;
                    }
                    else
                    {
                        index = (index + 1) % 2;
                    }
                }

                /* now tentatively claim the resource */
                flags[i] = ACTIVE;

                /* find the first active process besides ourselves, if any */
                index = 0;
                while ((index < 2) && ((index == i) || (flags[index] != ACTIVE)))
                {
                    index = index + 1;
                }

                /* if there were no other active processes, AND if we have the turn
	   or else whoever has it is idle, then proceed.  Otherwise, repeat
	   the whole sequence. */
            } while (!((index >= 2) && ((*turn == i) || (flags[*turn] == IDLE))));

            /* Start of CRITICAL SECTION */

            /* claim the turn and proceed */
            *turn = i;

            /* Critical Section Code of the Process */
            if (count % 2 == 0)
            {
                strcpy(text, string1);
            }
            else
            {
                strcpy(text, string2);
            }

            count++;

            /* End of CRITICAL SECTION */

            /* find a process which is not IDLE */
            /* (if there are no others, we will find ourselves) */
            index = (*turn + 1) % 2;
            while (flags[index] == IDLE)
            {
                index = (index + 1) % 2;
            }

            /* give the turn to someone that needs it, or keep it */
            *turn = index;

            /* we're finished now */
            flags[i] = IDLE;

            /* REMAINDER Section */
        }
    }
    else if (pid > 0)
    {

        while (1)
        {
            int i = 0;
            int index = 0;
            do
            {
                /* announce that we need the resource */
                flags[i] = WAITING;

                /* scan processes from the one with the turn up to ourselves. */
                /* repeat if necessary until the scan finds all processes idle */
                index = *turn;
                while (index != i)
                {
                    if (flags[index] != IDLE)
                    {
                        index = *turn;
                    }
                    else
                    {
                        index = (index + 1) % 2;
                    }
                }

                /* now tentatively claim the resource */
                flags[i] = ACTIVE;

                /* find the first active process besides ourselves, if any */
                index = 0;
                while ((index < 2) && ((index == i) || (flags[index] != ACTIVE)))
                {
                    index = index + 1;
                }

                /* if there were no other active processes, AND if we have the turn
	   or else whoever has it is idle, then proceed.  Otherwise, repeat
	   the whole sequence. */
            } while (!((index >= 2) && ((*turn == i) || (flags[*turn] == IDLE))));

            /* Start of CRITICAL SECTION */

            /* claim the turn and proceed */
            *turn = i;

            /* Critical Section Code of the Process */
            printf("%s\n", text);
sleep(2);
            /* End of CRITICAL SECTION */

            /* find a process which is not IDLE */
            /* (if there are no others, we will find ourselves) */
            index = (*turn + 1) % 2;
            while (flags[index] == IDLE)
            {
                index = (index + 1) % 2;
            }

            /* give the turn to someone that needs it, or keep it */
            *turn = index;

            /* we're finished now */
            flags[i] = IDLE;

            /* REMAINDER Section */
        }

        munmap(text, 1000);
        munmap(turn, sizeof(int));
        munmap(flags, sizeof(enum pstate));
    }
}