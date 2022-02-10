#include "../include/pcb.h"
#include "../include/yalnix.h"
#include <ykernel.h>
#include "../include/traps.h"

// Global Variables.
void *KERNEL_BRK;

// Set up a way to track free frames (use a bit vector to track this)
bit_vector_t mem_frames; // 0 for free, 1 for used.
unsigned int n_frames;

unsigned int brk_offset = 0; // keep track of allocated stuff.

// Set up the Kernel Page Table.
kernel_page_table_t kernel_page_table;

// Set up the Region 1 Page Table.
page_table_t region_1_page_table;

// Globals to store the idle and current processes
pcb_t idle_proc;
pcb_t current_proc;

// Flag to check whether vitual memory is enabled.
unsigned char is_virtual_memory_enabled = 0;

// The interrupt vector.
void *interrupt_vector[TRAP_VECTOR_SIZE] = {
    &trap_kernel_handler,
    &trap_clock_handler,
    &trap_illegal_handler,
    &trap_memory_handler,
    &trap_math_handler,
    &trap_tty_receive_handler,
    &trap_tty_transmit_handler,
    &trap_disk_handler,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL};

void PrintPageTable(kernel_page_table_t page_table)
{
    TracePrintf(0, "\n MAX_PT_LEN : %d\n", MAX_PT_LEN);
    for (int page_id = 0; page_id < MAX_PT_LEN; page_id++)
    {
        TracePrintf(0, "\n page id : %d \t| valid: %d \t | prot: %d \t| pfn: %d\n", page_id, page_table.table[page_id].valid, page_table.table[page_id].prot, page_table.table[page_id].pfn);
    }
}

int first_free_frame_idx()
{
    int i = 0;
    while (mem_frames.bit_arr[i] == 1)
    {
        i++;
    }
    return i;
}

extern int SetKernelBrk(void *addr)
{
    TracePrintf(0, "\n -------------------in SetKernelBrk--------------- \n");
    // Have a flag to check whether vitual memory is enabled
    // If it is not enabled, just track how much the kernel brk
    // is being raised past the _kernel_orig_brk
    if (is_virtual_memory_enabled == 0)
    {
        KERNEL_BRK = (void *)UP_TO_PAGE(addr);
        // Potentially deal with assigning new frames in case we malloc a lot after setting up the Region 0 Page Table.
    }
    // malloc() scenario:
    // After VM is enabled, if brk is raised.
    else if (is_virtual_memory_enabled == 1 && addr > KERNEL_BRK)
    {
        void *KERNEL_BRK_old = KERNEL_BRK;
        KERNEL_BRK = (void *)UP_TO_PAGE(addr);
        unsigned int page_id_start = (unsigned int)KERNEL_BRK_old >> PAGESHIFT;
        unsigned int page_id_end = (unsigned int)KERNEL_BRK >> PAGESHIFT;
        for (unsigned int page_id = page_id_start; page_id < page_id_end; page_id++)
        {
            // Get next free frame id.
            unsigned int next_free_frame_id = first_free_frame_idx();

            // Mark pte at page_id as valid and map it to the free_frame.
            kernel_page_table.table[page_id].valid = 1;
            kernel_page_table.table[page_id].prot = PROT_READ | PROT_WRITE;
            kernel_page_table.table[page_id].pfn = next_free_frame_id;
            mem_frames.bit_arr[next_free_frame_id] = 1; // Mark as used.
        }
    }
    // free() scenario:
    // After VM is enabled, if brk is lowered.
    else if (is_virtual_memory_enabled == 1 && addr < KERNEL_BRK)
    {
        void *KERNEL_BRK_old = KERNEL_BRK;
        KERNEL_BRK = (void *)UP_TO_PAGE(addr);
        unsigned int page_id_start = (unsigned int)KERNEL_BRK >> PAGESHIFT;
        unsigned int page_id_end = (unsigned int)KERNEL_BRK_old >> PAGESHIFT;
        for (unsigned int page_id = page_id_start; page_id < page_id_end; page_id++)
        {
            mem_frames.bit_arr[kernel_page_table.table[page_id].pfn] = 0; // Mark as free.

            // Mark pte at page_id as invalid and reset it.
            kernel_page_table.table[page_id].valid = 0;
            kernel_page_table.table[page_id].prot = PROT_NONE;
            kernel_page_table.table[page_id].pfn = 0;
        }
    }
    // Return 0 if success, ERROR if not
    else
    {
        return ERROR;
    }
    return 0;
}

void DoIdle()
{
    while (1)
    {
        TracePrintf(1, "\tDoIdle\n");
        Pause();
    }
}

void create_idle_process(UserContext *user_context)
{

    TracePrintf(0, "\n IN CREATE IDLE PROC\n");
    idle_proc.user_context = user_context;
    idle_proc.pid = helper_new_pid(region_1_page_table.table);

    // Allocate a frame for a process stack.
    int free_frame = first_free_frame_idx();

    // Set the page to valid.
    unsigned int stack_vpn = ((VMEM_1_LIMIT - VMEM_0_LIMIT) >> PAGESHIFT) - 1;
    region_1_page_table.table[stack_vpn].valid = 1;
    region_1_page_table.table[stack_vpn].prot = PROT_READ | PROT_WRITE;
    region_1_page_table.table[stack_vpn].pfn = free_frame;
    mem_frames.bit_arr[free_frame] = 1; // set that frame to used

    // Allocate a frame for a process stack.
    int free_frame_2 = first_free_frame_idx();

    // Set the page to valid.
    unsigned int stack_vpn_2 = ((VMEM_1_LIMIT - VMEM_0_LIMIT) >> PAGESHIFT) - 2;
    region_1_page_table.table[stack_vpn_2].valid = 1;
    region_1_page_table.table[stack_vpn_2].prot = PROT_READ | PROT_WRITE;
    region_1_page_table.table[stack_vpn_2].pfn = free_frame_2;
    mem_frames.bit_arr[free_frame_2] = 1; // set that frame to used

    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    // Set current instruction to DoIdle.
    idle_proc.user_context->pc = &DoIdle;
    // Set the stack pointer to the top of the stack.
    idle_proc.user_context->sp = (void *)(VMEM_1_LIMIT - (PAGESIZE));
    TracePrintf(0, "\n VMEM_1_LIMIT : %p\n", VMEM_1_LIMIT);
    TracePrintf(0, "\n stack_vpn : %d\n", stack_vpn);
    TracePrintf(0, "\n VMEM_1_LIMIT-1 & PAGEMASK : %p\n", VMEM_1_LIMIT - 1 & PAGEMASK);

    TracePrintf(0, "\n idle_proc.user_context->sp : %p\n", idle_proc.user_context->sp);
    // idle_proc.user_context->ebp = &region_1_page_table.table[stack_vpn];

    idle_proc.memory_context.user_page_table = region_1_page_table;
    idle_proc.memory_context.kernel_spage_table = kernel_page_table;
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

    // Initialize up KERNEL_BRK.
    KERNEL_BRK = _kernel_orig_brk;

    // Variables.
    n_frames = pmem_size / PAGESIZE;

    // Initializing all frames to free.
    mem_frames.bit_arr = malloc(sizeof(unsigned char) * n_frames);
    TracePrintf(0, "\n kernel data start %p\n", _kernel_data_start);
    TracePrintf(0, "\n kernel data end %p\n", _kernel_data_end);
    TracePrintf(0, "\npte addr %p\n", &kernel_page_table);
    TracePrintf(0, "\n free frames addr (mallocd) %p\n", mem_frames.bit_arr);
    brk_offset += sizeof(unsigned char) * n_frames;

    for (int i = 0; i < n_frames; i++)
    {
        mem_frames.bit_arr[i] = 0;
    }

    unsigned int page_id;
    unsigned int n_kernel_page_table_entries = VMEM_0_LIMIT >> PAGESHIFT;
    TracePrintf(0, "\n n_kernel_page_table_entries: %d \n", n_kernel_page_table_entries);
    for (unsigned int i = VMEM_0_BASE; i < VMEM_0_LIMIT; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t page_table_entry;

        // Mark the Kernel Text.
        if (VMEM_0_BASE <= i && i < (((int)_kernel_data_start & PAGEMASK)))
        {
            page_table_entry.valid = 1;
            page_table_entry.prot = PROT_READ | PROT_EXEC;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Data.
        else if ((((int)_kernel_data_start & PAGEMASK)) <= i && i < (unsigned int)_kernel_data_end)
        {
            page_table_entry.valid = 1;
            page_table_entry.prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Heap.
        else if ((unsigned int)_kernel_data_end <= i && i < ((unsigned int)KERNEL_BRK))
        {
            page_table_entry.valid = 1;
            page_table_entry.prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark the Kernel Stack.
        else if (KERNEL_STACK_BASE <= i && i < KERNEL_STACK_LIMIT)
        {
            page_table_entry.valid = 1;
            page_table_entry.prot = PROT_READ | PROT_WRITE;
            mem_frames.bit_arr[page_id] = 1; // Mark as used.
        }
        // Mark everything else as invalid.
        else
        {
            page_table_entry.valid = 0;
            page_table_entry.prot = 0;
        }
        page_table_entry.pfn = page_id;
        kernel_page_table.table[page_id] = page_table_entry;
    }
    // PrintPageTable(kernel_page_table);

    TracePrintf(0, "reg write: %p\n", kernel_page_table.table);
    TracePrintf(0, "reg limit: %d\n", n_kernel_page_table_entries);

    // Indicate the virtual memory base address of the kernel page table.
    WriteRegister(REG_PTBR0, (unsigned int)kernel_page_table.table);

    // Indicate the number of page table entries.
    WriteRegister(REG_PTLR0, n_kernel_page_table_entries);

    // Set up a Region 1 page table.
    page_id = 0;
    unsigned int n_region_1_page_table_entries = VMEM_1_SIZE / PAGESIZE;
    TracePrintf(0, "\n n_region_1_page_table_entries : %d \n", n_region_1_page_table_entries);
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        region_1_page_table.table[page_id] = page_table_entry;

        if (i == VMEM_1_LIMIT >> PAGESHIFT)
        {
            page_table_entry.valid = 1;
            page_table_entry.prot = PROT_READ | PROT_WRITE;
            page_table_entry.pfn = 0;
            region_1_page_table.table[page_id] = page_table_entry;
        }
    }

    // Indicate the virtual memory base address of the region 1 page table.
    WriteRegister(REG_PTBR1, (unsigned int)region_1_page_table.table);

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, n_region_1_page_table_entries);

    // Enable Virtual Memory.
    WriteRegister(REG_VM_ENABLE, 1);
    is_virtual_memory_enabled = 1;

    // Set up the interrupt register
    WriteRegister(REG_VECTOR_BASE, (unsigned int)interrupt_vector);

    create_idle_process(uctxt);

    TracePrintf(0, "\n DONSOS CHICOS\n");
}