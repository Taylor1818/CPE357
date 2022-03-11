#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

void swap(int *x, int *y)
{
   int temp;
   temp = *x;
   *x = *y;
   *y = temp;
}

void synch(int children, int *ready, int index)
{
   int flag = ready[2];
   ready[flag]++;

   while (ready[flag] != children);
   
   if (index == 0)
   {
      ready[1 - flag] = 0;
      ready[2] = 1 - flag;
   }
   else
   {
      while (ready[2] == flag);
   }
   return;
}

int isSorted(int *array, int size)
{
   for (int i = 0; i < size - 1; i++)
   {
      if (array[i] > array[i + 1])
      {
         return 0;
      }
   }
   return 1;
}

void printArray(int *array, int size)
{
   for (int i = 0; i < size; i++)
   {
      fprintf(stderr, "%d ", array[i]);
   }
}

void main(int argc, char *argv[])
{
   int *array = (int *)mmap(0, 2048, 0x1 | 0x2, 0x20 | 0x01, -1, 0);
   int size = 0;

   char chararray[100];

   while (scanf("%s", chararray) != EOF)
   {
      array[size] = atoi(chararray);
      size++;
   }

   fprintf(stderr, "Initial Array: [ ");
   printArray(array, size);
   fprintf(stderr, "]\n");

   int *ready = (int *)mmap(0, 3, 0x1 | 0x2, 0x20 | 0x01, -1, 0);
   ready[0] = 0;
   ready[1] = 0;
   ready[2] = 0;

   int maxChildren = size / 2;
   int children = atoi(argv[1]);

   if (children > maxChildren)
   {
      children = maxChildren;
   }

   clock_t start_t, total_t;

   for (int i = 0; i < children; i++)
   {
      int pid = fork();

      if (pid == 0)
      {
         int isOdd = 0;

         while (1)
         {
            int start = i * 2 + isOdd;

            for (int j = start; j < size - 1; j += children * 2)
            {
               if (array[j] > array[j + 1])
               {
                  swap(&array[j], &array[j + 1]);
               }
            }

            synch(children, ready, i);

            if (isSorted(array, size) == 1)
            {
               exit(0);
            }
            
            synch(children, ready, i);

            isOdd = 1 - isOdd;
         }
      }
   }

   for (int i = 0; i < children; i++)
   {
      wait(0);
   }

   fprintf(stderr, "Sorted Array:  [ ");
   printArray(array, size);
   fprintf(stderr, "]\n");

   fprintf(stderr, "\nProcesses: %d\n", children);

   total_t = ((clock() - start_t) * 1000 )/CLOCKS_PER_SEC;

   fprintf(stderr, "\nTime to Sort: %ldms\n", (total_t % 1000));

}