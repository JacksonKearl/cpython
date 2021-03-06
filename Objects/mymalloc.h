#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#include "memlib.h"

// All blocks must have a specified minimum alignment.
// The alignment requirement (from config.h) is >= 8 bytes.
#ifndef ALIGNMENT
#define ALIGNMENT 8
#endif

// Rounds up to the nearest multiple of ALIGNMENT.
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// The smallest aligned size that will hold a size_t value.
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// init - Initialize the malloc package.  Called once before any other
// calls are made.  Since this is a very simple implementation, we just
// return success.
int my_init()
{
    return 0;
}

//  malloc - Allocate a block by incrementing the brk pointer.
//  Always allocate a block whose size is a multiple of the alignment.
void *my_malloc(size_t size)
{
    // We allocate a little bit of extra memory so that we can store the
    // size of the block we've allocated.  Take a look at realloc to see
    // one example of a place where this can come in handy.
    int aligned_size = ALIGN(size + SIZE_T_SIZE);

    // Expands the heap by the given number of bytes and returns a pointer to
    // the newly-allocated area.  This is a slow call, so you will want to
    // make sure you don't wind up calling it on every malloc.
    void *p = mem_sbrk(aligned_size);

    if (p == (void *)-1)
    {
        // Whoops, an error of some sort occurred.  We return NULL to let
        // the client code know that we weren't able to allocate memory.
        return NULL;
    }
    else
    {
        // We store the size of the block we've allocated in the first
        // SIZE_T_SIZE bytes.
        *(size_t *)p = size;

        // Then, we return a pointer to the rest of the block of memory,
        // which is at least size bytes long.  We have to cast to uint8_t
        // before we try any pointer arithmetic because voids have no size
        // and so the compiler doesn't know how far to move the pointer.
        // Since a uint8_t is always one byte, adding SIZE_T_SIZE after
        // casting advances the pointer by SIZE_T_SIZE bytes.
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

// free - Freeing a block does nothing.
void my_free(void *ptr)
{
}

// realloc - Implemented simply in terms of malloc and free
void *my_realloc(void *ptr, size_t size)
{
    void *newptr;
    size_t copy_size;

    // Allocate a new chunk of memory, and fail if that allocation fails.
    newptr = my_malloc(size);
    if (NULL == newptr)
    {
        return NULL;
    }

    // realloc(NULL, ...) should be identical to malloc
    // The reference implemenation does not actually do this, so we don't expect you to,
    // but it is needed for using your malloc in CPython
    if (NULL == ptr)
    {
        return newptr;
    }

    // Get the size of the old block of memory.  Take a peek at my_malloc(),
    // where we stashed this in the SIZE_T_SIZE bytes directly before the
    // address we returned.  Now we can back up by that many bytes and read
    // the size.
    copy_size = *(size_t *)((uint8_t *)ptr - SIZE_T_SIZE);

    // If the new block is smaller than the old one, we have to stop copying
    // early so that we don't write off the end of the new block of memory.
    if (size < copy_size)
    {
        copy_size = size;
    }

    // This is a standard library call that performs a simple memory copy.
    memcpy(newptr, ptr, copy_size);

    // Release the old block.
    my_free(ptr);

    // Return a pointer to the new block.
    return newptr;
}
