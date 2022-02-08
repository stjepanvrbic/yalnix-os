#include "../include/pcb.h"
#include "../include/yalnix.h"

#include <ykernel.h>

// Set up a way to track free frames (use a bit vector to track this)
bit_vector_t mem_frames; // 0 for free, 1 for used.
// int n_frames = (VMEM_1_SIZE / __SIZEOF_INT__);
int n_frames = 1000;

// Set up the Kernel Page Table.
kernel_page_table_t kernel_page_table;

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
    TracePrintf(0, "\n_____________YOOOO___________ In KernelStart\n");

    int i = 0;
    while (cmd_args[i] != NULL)
    {
        TracePrintf(0, "\n cmd_args[%d] : %s \n", i, cmd_args[i]);
        i++;
    }

    TracePrintf(0, "\n pmem_size : %d \n", pmem_size);

    TracePrintf(0, "\n UserContext.vector : %d \n", uctxt->vector);
    TracePrintf(0, "\n UserContext.code : %d \n", uctxt->code);
    TracePrintf(0, "\n UserContext.addr : %p \n", uctxt->addr);
    TracePrintf(0, "\n UserContext.pc : %p \n", uctxt->pc);
    TracePrintf(0, "\n UserContext.sp : %p \n", uctxt->sp);
    TracePrintf(0, "\n UserContext.ebp : %p \n", uctxt->ebp);
    i = 0;
    while (i < GREGS)
    {
        TracePrintf(0, "\n UserContext.regs[%d] : %lu \n", i, uctxt->regs[i]);
        i++;
    }

    TracePrintf(0, "\n n_frames : %d \n", n_frames);
    // Initializing all frames to free.
    for (int i = 0; i < n_frames; i++)
    {
        mem_frames.bit_arr[i] = 0;
    }

    // Set up the Region 0 page table (kernel heap and stack)
    TracePrintf(0, "\n _kernel_data_start : %p \n", _kernel_data_start);
    TracePrintf(0, "\n _kernel_data_end : %p \n", _kernel_data_end);
    TracePrintf(0, "\n _kernel_orig_brk : %p \n", _kernel_orig_brk);

    u_long page_id;
    u_long n_kernel_frames = pmem_size / PAGESIZE;
    for (u_long i = PMEM_BASE; i < pmem_size; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t *page_table_entry = malloc(sizeof(pte_t));

        // Mark the Kernel Text.
        if (PMEM_BASE <= i && i < _kernel_data_start)
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ & PROT_EXEC;
        }
        // Mark the Kernel Data.
        else if (_kernel_data_start <= i && i < _kernel_data_end)
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ & PROT_WRITE;
        }
        // Mark the Kernel Heap.
        else if (_kernel_data_end <= i && i < (_kernel_orig_brk + sizeof(pte_t) * n_kernel_frames))
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ & PROT_WRITE;
        }
        // Mark everything else as invalid.
        else
        {
            page_table_entry->valid = 0;
            page_table_entry->prot = 0;
        }

        page_table_entry->pfn = page_id;
        kernel_page_table.table[page_id] = page_table_entry;
    }

    // Set up a Region 1 page table for the idle process */
}