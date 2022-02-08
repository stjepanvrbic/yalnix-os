#include "../include/pcb.h"
#include "../include/yalnix.h"
#include <ykernel.h>

// Set up a way to track free frames (use a bit vector to track this)
bit_vector_t mem_frames; // 0 for free, 1 for used.
u_long n_frames;

// Set up the Kernel Page Table.
kernel_page_table_t kernel_page_table;

// Set up the Region 1 Page Table.
kernel_page_table_t region_1_page_table;

// Flag to check whether vitual memory is enabled.
unsigned char is_virtual_memory_enabled = 0;

extern int SetKernelBrk(void *addr)
{
    TracePrintf(0, "\n SETBRK CALLED\n");
    // Have a flag to check whether vitual memory is enabled
    // If it is not enabled, just track how much the kernel brk
    // is being raised past the _kernel_orig_brk
    // if (is_virtual_memory_enabled == 0) {

    // }
    // After VM is enabled, act like standard Brk
    // Enable virtual memory, keeping track of whether the original brk
    // was raised
    // Return 0 if success, ERROR if not
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

    // Variables.
    u_long brk_offset = 0; // keep track of allocated stuff.
    n_frames = pmem_size / PAGESIZE;
    TracePrintf(0, "\n n_frames : %d \n", n_frames);

    // Initializing all frames to free.
    mem_frames.bit_arr = malloc(sizeof(unsigned char) * n_frames);
    brk_offset += sizeof(unsigned char) * n_frames;
    for (int i = 0; i < n_frames; i++)
    {
        mem_frames.bit_arr[i] = 0;
    }

    // Set up the Region 0 page table (kernel heap and stack)
    TracePrintf(0, "\n _kernel_data_start : %p \n", _kernel_data_start);
    TracePrintf(0, "\n _kernel_data_end : %p \n", _kernel_data_end);
    TracePrintf(0, "\n _kernel_orig_brk : %p \n", _kernel_orig_brk);

    u_long page_id;
    u_long n_kernel_page_table_entries = VMEM_0_SIZE / PAGESIZE;
    TracePrintf(0, "\n n_kernel_page_table_entries : %d \n", n_kernel_page_table_entries);
    for (u_long i = VMEM_0_BASE; i < VMEM_0_LIMIT; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t *page_table_entry = malloc(sizeof(pte_t));
        if (page_table_entry == NULL)
        {
            TracePrintf(0, "\nFatal: failed to allocate bytes. \n");
        }
        brk_offset += sizeof(pte_t);

        // Mark the Kernel Text.
        if (VMEM_0_BASE <= i && i < ((u_long)_kernel_data_start))
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ | PROT_EXEC;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Data.
        else if ((u_long)_kernel_data_start <= i && i < (u_long)_kernel_data_end)
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Heap.
        else if ((u_long)_kernel_data_end <= i && i < ((u_long)_kernel_orig_brk + brk_offset))
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Stack.
        else if (KERNEL_STACK_BASE <= i && i < KERNEL_STACK_LIMIT)
        {
            page_table_entry->valid = 1;
            page_table_entry->prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
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

    // Indicate the virtual memory base address of the kernel page table.
    WriteRegister(REG_PTBR0, (unsigned int)&kernel_page_table);

    // Indicate the number of page table entries.
    WriteRegister(REG_PTLR0, n_kernel_page_table_entries);

    // Set up a Region 1 page table.
    page_id = 0;
    u_long n_region_1_page_table_entries = VMEM_1_SIZE / PAGESIZE;
    TracePrintf(0, "\n n_region_1_page_table_entries : %d \n", n_region_1_page_table_entries);
    for (u_long i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t *page_table_entry = malloc(sizeof(pte_t));
        brk_offset += sizeof(pte_t);

        // Mark everything as invalid.
        page_table_entry->valid = 0;
        page_table_entry->prot = 0;
        page_table_entry->pfn = page_id;
        region_1_page_table.table[page_id] = page_table_entry;
    }

    // Indicate the virtual memory base address of the region 1 page table.
    WriteRegister(REG_PTBR1, (unsigned int)&region_1_page_table);

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, n_region_1_page_table_entries);

    // Enable Virtual Memory.
    // WriteRegister(REG_VM_ENABLE, 1);

    TracePrintf(0, "\n DONSOS CHICOS\n");
}