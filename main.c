#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define IS_DEBUG 1

struct mem
{
    size_t size;
    bool isFree;
    void *CurrAddr;
    struct mem *PrevAddr;
    struct mem *NextAddr;

} mem;

static struct mem MemoryHead;
static struct mem *MemoryTail;

void *
ASL_Malloc (size_t size)
{

    void *ptr;
    struct mem *Head = &MemoryHead;

    while (Head)
    {
        if (Head->size == size && Head->isFree)
        {
            Head->isFree = false;
            return Head->CurrAddr;
        }
        Head = Head->NextAddr;
    }

    ptr = sbrk (size);
    if (ptr == (void *)-1)
    {
        printf ("error out of mem\n");
        return NULL;
    }

#if IS_DEBUG
    printf ("TEST: %ld\n", MemoryHead.size);
#endif

    if (MemoryHead.size == 0)
    {


#if IS_DEBUG
        printf ("TEST: CREATE NEW HEAD \n");
#endif

        MemoryHead.CurrAddr = ptr;
        MemoryHead.size = size;
        MemoryHead.NextAddr = NULL;
        MemoryHead.PrevAddr = NULL;
        MemoryTail = &MemoryHead;

#if IS_DEBUG
        printf ("TEST: HEAD ADDR: %p \n", MemoryHead.CurrAddr);
#endif

    
        return MemoryHead.CurrAddr;
    }
    else
    {
       
        struct mem *NewTail = sbrk(sizeof(struct mem));
        NewTail->NextAddr = NULL;
        NewTail->PrevAddr = NULL;
        NewTail->CurrAddr = ptr;
        NewTail->size = size;
        NewTail->PrevAddr = MemoryTail;
        MemoryTail->NextAddr = NewTail;
        MemoryTail = MemoryTail->NextAddr;

#if IS_DEBUG
        printf ("TEST: NEW TAIL ADDR: %p PREV: %p SIZE:%ld \n", NewTail->CurrAddr, NewTail->PrevAddr, NewTail->size);
#endif

       
        return MemoryTail->CurrAddr;
    }

    return NULL;
}

void
ASL_free (void *ptr)
{
    struct mem *Head = &MemoryHead;
    while (Head)
    {
        if (Head->CurrAddr == ptr)
        {
            Head->isFree = true;
        }
        Head = Head->NextAddr;
    }
}

void
listMem ()
{
    struct mem *Head = &MemoryHead;
    putchar ('\n');
    while (Head)
    {

        printf ("ADDR:%p ADDR.C: %p size %ld FREE?:%d |P:%p N:%p| \n",
            Head,
            Head->CurrAddr,
            Head->size, Head->isFree,
            Head->PrevAddr, Head->NextAddr);

        Head = Head->NextAddr;
    }
}
int
main ()
{
    void *a = ASL_Malloc (10);
    listMem ();
    printf ("%p \n", a);
    ASL_free (a);
    listMem ();
    
    int *b =(int*) ASL_Malloc (11);
    b[0] = 20;
    listMem ();
    return 0;
}