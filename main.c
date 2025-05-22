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
static struct mem MemoryTail;

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
        #if IS_DEBUG
        printf ("TEST: HEAD ADDR: %p \n",MemoryHead.CurrAddr);
#endif
        MemoryTail = MemoryHead;

        return MemoryHead.CurrAddr;
    }
    else
    {
       
        struct mem NewTail;
        NewTail.CurrAddr = ptr;
        NewTail.size = size;
        NewTail.PrevAddr = &MemoryTail;


        #if IS_DEBUG
        printf ("TEST: NEW TAIL ADDR: %p PREV: %p\n",NewTail.CurrAddr,NewTail.PrevAddr);
#endif



        MemoryTail = NewTail;

        return NewTail.CurrAddr;
    }

    return NULL;
}

void 
ASL_free( void* ptr)
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
    putchar('\n');
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
    //TODO некорректно выделяется новая память, косяк в связи хвоста и головы 
    void *a = ASL_Malloc(10);
    listMem();
    printf("%p \n", a );
    ASL_free(a);
    listMem();
    a = ASL_Malloc(11);
    listMem();
    return 0;
}