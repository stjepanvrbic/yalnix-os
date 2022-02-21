/* hardware.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30
 * Version: 1.0
 *
 * Description: Implements the kernel start and set kernel brk functions.
 */

#include "../include/utils.h"
#include "../include/pcb.h"
#include "../include/traps.h"
#include "../include/hardware.h"
#include "../include/kernel_context.h"
#include "../include/load_program.h"
#include "../include/queue.h"

#include <ykernel.h>
#include <yalnix.h>

// Global Variables.
void *KERNEL_BRK;
UserContext *curr_uctxt;

bit_vector_t mem_frames; // 0 for free, 1 for used.

kernel_page_table_t kernel_page_table;

queue_t *ready_queue;
queue_t *blocked_queue;

// Set up the Region 1 Page Table.
page_table_t *region_1_page_table;

// Globals to keep track of processes.
pcb_t *curr_pcb;
pcb_t idle_pcb;

unsigned int n_frames;

// Keep track of allocated stuff.
unsigned int brk_offset = 0;

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

/****************************************************** HELPERS ******************************************************/

//---------------------------- PrintKernelPageTable ------------------------------
// Description: Prints an entire kernel page table.
// Inputs:      The kernel page table to be printed.
// Outputs:     The kernel page table is printed to the console
//--------------------------------------------------------------------------------
void PrintKernelPageTable(page_table_t *page_table)
{
    for (int page_id = 0; page_id < MAX_PT_LEN; page_id++)
    {
        TracePrintf(0, "\n page id : %d \t| valid: %d \t | prot: %d \t| pfn: %d\n", page_id, page_table->table[page_id].valid, page_table->table[page_id].prot, page_table->table[page_id].pfn);
    }
}

//---------------------------- PrintUserPageTable --------------------------------
// Description: Prints an entire user page table.
// Inputs:      The user page table to be printed.
// Outputs:     The user page table is printed to the console
//--------------------------------------------------------------------------------
void PrintUserPageTable(page_table_t *page_table)
{
    for (int page_id = 0; page_id < MAX_PT_LEN; page_id++)
    {
        TracePrintf(0, "\n page id : %d \t| valid: %d \t | prot: %d \t| pfn: %d\n", page_id, page_table->table[page_id].valid, page_table->table[page_id].prot, page_table->table[page_id].pfn);
    }
}

//---------------------------- first_free_frame_idx ------------------------------
// Description: Finds the first free frame in physical memory and marks it as used.
// Inputs:      None.
// Outputs:     Returns the index of the first free frame in the bit array.
//--------------------------------------------------------------------------------
int first_free_frame_idx()
{
    int i = 0;
    while (mem_frames.bit_arr[i] == 1)
    {
        i++;
    }
    mem_frames.bit_arr[i] = 1; // Mark is at used.
    return i;
}

/*************************************************** PROCESSES ****************************************************/

//------------------------------------ DoIdle ------------------------------------
// Description: Function that makes a process go idle.
// Inputs:      None.
// Outputs:     Outputs DoIdle to the console on every clock tick.
//--------------------------------------------------------------------------------
void DoIdle()
{
    while (1)
    {
        TracePrintf(1, "\tDoIdle\n");
        Pause();
    }
}

//------------------------------- create_idle_process ---------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void create_idle_process(UserContext *user_context, void (*func)())
{
    idle_pcb.pid = helper_new_pid((struct pte *)&region_1_page_table->table);

    // Allocate a frame for a process stack.
    int free_frame = first_free_frame_idx();

    // Set the last page to valid.
    unsigned int stack_vpn = ((VMEM_1_LIMIT - VMEM_0_LIMIT) >> PAGESHIFT) - 1;
    region_1_page_table->table[stack_vpn].valid = 1;
    region_1_page_table->table[stack_vpn].prot = PROT_READ | PROT_WRITE;
    region_1_page_table->table[stack_vpn].pfn = free_frame;

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    // Set current instruction to DoIdle.
    user_context->pc = func;

    // Set the stack pointer to the top of the stack.
    user_context->sp = (void *)SP_ADD_OFFSET(VMEM_1_LIMIT); // The stack pointer points 0x20 bytes below the high address of the last page in the stack.

    // Set the page tables of the idle pcb
    idle_pcb.memory_context.user_page_table = region_1_page_table;

    // Copy a copy of the user context into the pcb for idle_pcb
    idle_pcb.user_context = *user_context;

    curr_uctxt = user_context;
}

//------------------------------ new_kernel_stack --------------------------------
// Description: Initialize two new frames for a kernel stack and mark them as valid.
// Inputs:      None.
// Outputs:     A kernel stack table is initilaized and marked as valid.
//              That kernel stack table is returned.
//--------------------------------------------------------------------------------
kernel_stack_t new_kernel_stack()
{
    kernel_stack_t temp_stack;
    pte_t page0;
    pte_t page1;

    // Find free frames in physical memory
    u_long pfn0 = first_free_frame_idx();
    u_long pfn1 = first_free_frame_idx();

    // Initialize the page table entries as valid
    page0.pfn = pfn0;
    page0.valid = 1;
    page0.prot = PROT_READ | PROT_WRITE;
    page1.pfn = pfn1;
    page1.valid = 1;
    page1.prot = PROT_READ | PROT_WRITE;

    // Copy the page table entries into the kernel stack
    temp_stack.table[0] = page0;
    temp_stack.table[1] = page1;

    return temp_stack;
}

//-------------------------------- create_process ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void create_process(UserContext *user_context, pcb_t *new_pcb)
{
    // FREE PAGE TABLE WHEN RETIRING PROCESS
    page_table_t *temp_page_table = malloc(sizeof(page_table_t));
    unsigned int page_id = 0;
    // Set up a Region 1 page table.
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        page_id = ((i - VMEM_1_BASE) / PAGESIZE);
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        temp_page_table->table[page_id] = page_table_entry;
    }
    // Set up the brk
    new_pcb->memory_context.brk = (void *)VMEM_1_BASE;

    // Set the pointer of the user stack
    new_pcb->memory_context.region_1_sp = NULL;

    // Set the user page table
    new_pcb->memory_context.user_page_table = temp_page_table;

    // Set up a Kernel stack table.
    new_pcb->memory_context.kernel_stack = new_kernel_stack();

    // Get a PID for the process
    new_pcb->pid = helper_new_pid((struct pte *)&new_pcb->memory_context.user_page_table->table);

    // Set the parent pcb to null
    new_pcb->parent_pcb_p = NULL;

    // Make a copy of the user context in the init PCB
    new_pcb->user_context = *user_context;

    // Initiliaze the queues
    new_pcb->children = qopen();
    new_pcb->deceased_children = qopen();
}

/*************************************************** START KERNEL ****************************************************/

// CAUTION: WE ADDED TOO MUCH FUNCTIONALITY TO SETKERNELBRK, IT SHOULD ONLY CHANGE THE KERNEL BRK AND NOT DEAL WITH MEMORY
extern int SetKernelBrk(void *addr)
{
    TracePrintf(0, "\n ------------------- In SetKernelBrk --------------- \n");
    // Have a flag to check whether vitual memory is enabled
    // If it is not enabled, just track how much the kernel brk 
    // is being raised past the _kernel_orig_brk
    if (is_virtual_memory_enabled == 0)
    {
        // Keep track of the offset
        brk_offset = (unsigned int)UP_TO_PAGE(addr) - (unsigned int)KERNEL_BRK;
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
        TracePrintf(0, "\n ------------------- In SetKernelBrk : ERRROR --------------- \n");
        return ERROR;
    }
    TracePrintf(0, "\n ------------------- Exiting SetKernelBrk --------------- \n");
    return 0;
}

extern void KernelStart(char **cmd_args, unsigned int pmem_size, UserContext *uctxt)
{
    TracePrintf(0, "\n--------------- In KernelStart ---------------\n");
    int i = 0;

    // Initialize queues
    ready_queue = qopen();
    blocked_queue = qopen();

    // Initialize up KERNEL_BRK.
    KERNEL_BRK = _kernel_orig_brk;

    // Variables.
    n_frames = pmem_size / PAGESIZE;

    // Initializing all frames to free.
    mem_frames.bit_arr = malloc(sizeof(unsigned char) * n_frames);
    // brk_offset += sizeof(unsigned char) * n_frames;
    for (int i = 0; i < n_frames; i++)
    {
        mem_frames.bit_arr[i] = 0;
    }

    unsigned int page_id;
    unsigned int n_kernel_page_table_entries = VMEM_0_LIMIT >> PAGESHIFT;
    // Set up the kernel page table
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

    // Indicate the virtual memory base address of the kernel page table.
    WriteRegister(REG_PTBR0, (unsigned int)kernel_page_table.table);

    // Indicate the number of page table entries.
    WriteRegister(REG_PTLR0, n_kernel_page_table_entries);

    page_id = 0;
    region_1_page_table = malloc(sizeof(page_table_t));
    // Set up a Region 1 page table.
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        page_id = ((i - VMEM_1_BASE) / PAGESIZE);
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        region_1_page_table->table[page_id] = page_table_entry;
    }

    // Indicate the virtual memory base address of the region 1 page table.
    WriteRegister(REG_PTBR1, (unsigned int)region_1_page_table->table);

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, N_R1_PTE_ENTRIES);

    // Enable Virtual Memory.
    WriteRegister(REG_VM_ENABLE, 1);
    is_virtual_memory_enabled = 1;

    // Set up the interrupt register
    WriteRegister(REG_VECTOR_BASE, (unsigned int)interrupt_vector);

    // Create idle process pcb and run the idle function
    TracePrintf(0, "\n --------------- IN CREATE IDLE PROC ---------------\n");
    create_idle_process(uctxt, &DoIdle);
    TracePrintf(0, "\n--------------- LEFT CREATE IDLE PROC ---------------\n");

    // Bookkeping to keep track of the currently running process
    curr_pcb = &idle_pcb;

    // Create init process pcb and map the region 1 page table to it
    TracePrintf(0, "\n--------------- IN CREATE INIT PROC ---------------\n");
    pcb_t *init_pcb = malloc(sizeof(pcb_t));
    create_process(uctxt, init_pcb);
    TracePrintf(0, "\n--------------- LEFT CREATE INIT PROC ---------------\n");

    // Add the init process to the queue of ready processes.
    int status = (int)qput(ready_queue, (void *)init_pcb);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- ERROR : Adding pcb to queue FAILED ---------------\n");
    }

    // Clone idle into init.
    TracePrintf(0, "\n--------------- in kernel start | About to Clone IDLE into INIT ---------------\n");
    status = KernelContextSwitch(KCCopy, (void *)init_pcb, NULL); // NOTE: both idle and init will read the rest of the function.
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
        return;
    }

    // If INIT PCB is running this end of Kernal Start:
    if (curr_pcb->pid == init_pcb->pid)
    {
        // Load init executable into the region 1 page table of the init process
        TracePrintf(0, "\n--------------- in kernel start | I'M INIT ---------------\n");
        TracePrintf(0, "\n--------------- in kernel start | About to Load INIT ---------------\n");

        // Indicate the virtual memory base address of the region 1 page table.
        WriteRegister(REG_PTBR1, (unsigned int)init_pcb->memory_context.user_page_table->table);

        // Indicate the number of page table entries in the region 1 page table.
        WriteRegister(REG_PTLR1, N_R1_PTE_ENTRIES);

        curr_pcb->parent_pcb_p = &idle_pcb;

        // If no init program is provided, run the base init program.
        if (cmd_args[0] == NULL)
        {
            status = LoadProgram("./test/init", cmd_args, init_pcb);
        }
        else
        {
            status = LoadProgram(cmd_args[0], cmd_args, init_pcb);
        }
        if (status != 0)
        {
            TracePrintf(0, "\n--------------- Load Program Failed ---------------\n");
            return;
        }
        *uctxt = init_pcb->user_context;
        TracePrintf(0, "\n--------------- in kernel start | Back from Loading INIT? ---------------\n");
    }

    // Flush TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    TracePrintf(0, "\n ==================== Leaving KernelStart =====================\n");
}