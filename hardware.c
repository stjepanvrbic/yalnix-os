#include "include/hardware.h"
#include <ykernel.h>

extern int SetKernelBrk(void *addr)
{
    /* Have a flag to check whether vitual memory is enabled
    If it is not enabled, just track how much the kernel brk 
    is being raised past the _kernel_orig_brk
    After VM is enabled, act like standard Brk
    Enable virtual memory, keeping track of whether the original brk
    was raised
    Return 0 if success, ERROR if not */
}

/* This is the primary entry point into the kernel */
extern void KernelStart(char **cmd_args, unsigned int pmem_size, UserContext *uctxt)
{
    /* Set up a way to track free frames (use a bit vector to track this)
    Set up the Region 0 page table (kernel heap and stack)
    Set up a Region 1 page table for the idle process */
}