#include <stdio.h>
#include <stdlib.h>

#define PAGESIZE 1024

unsigned char myheap[1048576];

struct chunkhead
{
    unsigned int size;
    unsigned int info;
    struct chunkhead *next, *prev;  
}chunkhead;

struct chunkhead *head;

char* mymalloc(int size)
{
    if(head == NULL){
        head = (struct chunkhead*) &myheap[0];
        head -> size = sizeof(myheap);
        head -> info = 0;
        head -> next = 0;
        head -> prev = 0;
    }

    //bad or 0 as input
    if(size <= 0){
        return 0;
    }

    struct chunkhead *address = head;

    //handle chunkhead
    size += sizeof(chunkhead);

    int pages = 1 + size/PAGESIZE;
    
    //size divisable by pagesize
    if( size % PAGESIZE)
    {
        size = PAGESIZE * pages;
    }

    while(address){

        //chunk available
        if(address -> size >= size && !address -> info)
        {
            
            int sizeDifference = address -> size - size;
            address -> size = size; // new size of address
            address -> info = 1; //chunk now occupied

            if(sizeDifference)
            {
                struct chunkhead *newNext = (struct chunkhead*) address+size;
                newNext -> size = sizeDifference;
                newNext -> info = 0;
                newNext -> prev = address;
                
                if(address -> next)
                {
                    newNext -> next = address -> next;
                    newNext -> next -> prev = newNext;
                }
                address -> next = newNext;
            }
            return (char*)(address+1);
        }
        else
        {
            address = address -> next;
        }
    }
    return 0;
}

void myfree(char *address)
{
    struct chunkhead *addr = (struct chunkhead*) address;

    //ititialize empty page
    addr -= 1;
    addr -> info = 0;

    // empty previous case
    if(addr -> prev && !addr -> prev -> info)
    {
        addr -> prev -> size = addr -> size + addr -> prev -> size;
        addr -> prev -> next = addr -> next;

        if(addr -> next)
        {
            addr -> next -> prev = addr -> prev;
        }
        addr = addr -> prev;
    }

    // empty next case
    if(addr -> next && !addr -> next -> info)
    {
        addr -> size = addr -> size + addr -> next -> size;
        addr -> next = addr -> next -> next;
        
        if( addr -> next )
        {
            addr -> next -> prev = addr;
        }
    }
}

void analyze()
{
    struct chunkhead *address = head;

    int i = 1;

    while (address)
    {
        printf("\nChunk #%d: \n", i);
        printf("Size = %d\t bytes", address -> size);
        
        if(address -> info)
        {
            printf("\nOccupied");
        }
        else
        {
            printf("\nFree");
        }

        printf("\nNext = %p\n", address -> next);
        printf("Prev = %p\n", address -> prev);

        i++;
        address = address -> next;
    }   
}

void main()
{
    unsigned char *a, *b, *c;
    a = mymalloc(1000);
    b = mymalloc(2000);
    c = mymalloc(1000);

    myfree(b);
    myfree(a);

    analyze();
}
