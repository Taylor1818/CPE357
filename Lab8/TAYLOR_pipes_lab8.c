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

typedef unsigned char byte;

typedef struct mypipe
{
    byte *pipebuffer;
    int buffersize;
    int start_occupied;
    int end_occupied;
} mypipe;

//initializes (malloc) the pipe with a size of "size" and sets start and end.
void init_pipe(mypipe *pipe, int size);

// //writes "size" bytes from buffer into the pipe, returns size
int mywrite(mypipe *pipe, byte *buffer, int size);

// //reads "size" bytes from pipe into buffer, returns how much it read (max size), 0 if
// pipe is empty
int myread(mypipe *pipe, byte *buffer, int size);

void main()
{
    char text[100];
    mypipe pipeA;

    init_pipe(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);

    myread(&pipeA, text, 12);
    printf("%s\n", text);
    myread(&pipeA, text, 16);
    printf("%s\n", text);

    mywrite(&pipeA, "and now we test the carryover", 30);
    myread(&pipeA, text, 30);
    printf("%s\n", text);
}

void init_pipe(mypipe *pipe, int size)
{
    pipe -> pipebuffer = malloc(size);
    pipe -> buffersize = size;
    pipe -> start_occupied = 0;
    pipe -> end_occupied = 0;
}

int mywrite(mypipe *pipe, byte *buffer, int size)
{
    int count = 0;
    int end = pipe->end_occupied;

    if (size > pipe->buffersize)
    {
        return count;
    }

    for (int i = end; i <= (end + size); i++)
    {
        if((i + 1) % pipe->buffersize == pipe-> start_occupied ){
            break;
        }
        pipe->pipebuffer[i % pipe->buffersize] = buffer[count];
        count += 1;
    }
    
    pipe->end_occupied = (end + size) % pipe->buffersize;

    return count;
}

int myread(mypipe *pipe, byte *buffer, int size)
{
    int count = 0;
    int start = pipe->start_occupied;

    if (size > pipe->buffersize || start == pipe->end_occupied)
    {
        return count;
    }

    for (int i = start; i < (start + size); i++)
    {
        if( (i % pipe->buffersize) == pipe-> end_occupied){
            break;
        }
        buffer[ count ] = pipe->pipebuffer[ i % pipe->buffersize ];
        count += 1;
    }
    
    pipe->start_occupied = (start + size) % pipe->buffersize;
    
    return count;
}
