#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define PAGESIZE 4096

typedef unsigned char byte;

struct chunkhead
{
    unsigned int size;
    unsigned int info;
    struct chunkhead *next,*prev;
}chunkhead;

struct chunkhead *head;
struct chunkhead *last;

struct chunkhead* findBestFit(int size)
{
    struct chunkhead* addr = head;
    struct chunkhead* minAddr = 0;
    while(1)
    {
        if(!addr->info)
        {
            if(addr->size == size)
            {
                return addr;
            } 
            else if(addr->size > size && minAddr == 0 || minAddr->size > addr->size){//valid space
                minAddr = addr;
            }
        }
        if(!addr->next){
            last = addr;
            break;
        }
        addr = addr->next;

    }

    if(minAddr == 0){
        return NULL;
    }
}

char* mymalloc(int size)
{
    if(size<=0)
    {
        return 0;
    }
    
    struct chunkhead *addr = head;

    size += sizeof(chunkhead);

    int pages = 1 + size/PAGESIZE;

    if(size % PAGESIZE)
    {
        size = PAGESIZE*pages; 
    }

    if(!head)
    {
        printf("%c",0);
        head = sbrk(size);
        head->size=size;     
        head->info=1;          
        head->next=0;head->prev=0;
        return (char *)(head +1);
    }

    struct chunkhead* best = findBestFit(size);

    if(best)
    {
        addr = best;
        int sizeDif = addr->size - size;        
        addr->size=size;                          
        addr->info=1;                           

        if(sizeDif)
        {                            
            struct chunkhead *newNext = (struct chunkhead*) addr+size;
            newNext->size = sizeDif;
            newNext->info = 0;
            newNext->prev = addr;

            if(addr->next)
            {
                newNext->next = addr->next;
                newNext->next->prev = newNext;
            }
            addr->next = newNext;               
        }
        return (char*)(addr+1);
    }

    addr = last;

    struct chunkhead* new = sbrk(size);

    new->info = 1;
    new->size = size;
    new->next=0;
    new->prev=addr;

    addr->next=new;

    return (char *)(new+1);                                     
}

void myfree(char *address)
{
    struct chunkhead *addr = (struct chunkhead*)address;
    addr -= 1;

    if(!address)
    {
        return; 
    }

    if(addr == head && !head->next)
    {
        head = 0;
        brk(sbrk(0)-addr->size);
        return;
    }

    addr->info = 0;

    if(addr->prev && !addr->prev->info)
    {
        addr->prev->size = addr->size+addr->prev->size;
        addr->prev->next = addr->next;
        if(addr->next) addr->next->prev = addr->prev;
        addr = addr->prev;
    }

    if(addr->next)
    {
        if(!addr->next->info)
        {
            addr->size = addr->size+addr->next->size;
            addr->next = addr->next->next;

            if(addr->next)
            {
                addr->next->prev = addr;
            }
        }
    }

    if(!addr->next)
    {
        if(addr == head)
        {
            head = 0;
        } else
        {
            addr->prev->next = 0;
        }

        brk(sbrk(0)-addr->size);

        return;
    }
}

void analyze()
{
    printf("\n--------------------------------------------------------------\n");
    
    if(!head)
    {
        printf("no heap\n");
        return;
    }
    struct chunkhead* ch = (struct chunkhead*)head;

    for(int no = 0; ch; ch = (struct chunkhead*)ch->next, no++)
    {
        printf("% 3d | current addr: %p |", no, ch);
        printf("size: % 10d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %p | ", ch->next);
        printf("prev: %p", ch->prev);
        printf("      \n");
    }

    printf("program break on address: %p\n",sbrk(0));
}

void test()
{

    byte* a[100];

    analyze();

    for(int i=0;i<100;i++)
    {
        a[i]= mymalloc(1000);
    }

    for(int i=0;i<90;i++)
    {
        myfree(a[i]);
    }

    analyze();

    myfree(a[95]);

    a[95] = mymalloc(1000);

    analyze(); 

    for(int i=90;i<100;i++)
    {
        myfree(a[i]);
    }

    analyze();
}

void testTime()
{
    byte* a[100];

    clock_t ca, cb;
    ca = clock();

    for(int i=0;i<100;i++){
        a[i]= mymalloc(1000);
    }
    for(int i=0;i<90;i++){
        myfree(a[i]);
    } 

    myfree(a[95]);

    a[95] = mymalloc(1000);

    for(int i=90;i<100;i++)
    {
        myfree(a[i]);
    }

    cb = clock();

    printf("\nduration: %ld\n", (cb -ca));
}

void main()
{
    test();
    testTime();
}