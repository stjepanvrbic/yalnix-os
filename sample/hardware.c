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
#include <ykernel.h>
#include <yalnix.h>

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
void PrintKernelPageTable(page_table_t page_table)
{
    for (int page_id = 0; page_id < MAX_PT_LEN; page_id++)
    {
        TracePrintf(0, "\n page id : %d \t| valid: %d \t | prot: %d \t| pfn: %d\n", page_id, page_table.table[page_id].valid, page_table.table[page_id].prot, page_table.table[page_id].pfn);
    }
}

//---------------------------- PrintUserPageTable --------------------------------
// Description: Prints an entire user page table.
// Inputs:      The user page table to be printed.
// Outputs:     The user page table is printed to the console
//--------------------------------------------------------------------------------
void PrintUserPageTable(page_table_t page_table)
{
    for (int page_id = 0; page_id < MAX_PT_LEN; page_id++)
    {
        TracePrintf(0, "\n page id : %d \t| valid: %d \t | prot: %d \t| pfn: %d\n", page_id, page_table.table[page_id].valid, page_table.table[page_id].prot, page_table.table[page_id].pfn);
    }
}

//---------------------------- first_free_frame_idx ------------------------------
// Description: Finds the first free frame in physical memory.
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
    idle_pcb.pid = helper_new_pid(region_1_page_table.table);

    // Allocate a frame for a process stack.
    int free_frame = first_free_frame_idx();

    // Set the last page to valid.
    unsigned int stack_vpn = ((VMEM_1_LIMIT - VMEM_0_LIMIT) >> PAGESHIFT) - 1;
    region_1_page_table.table[stack_vpn].valid = 1;
    region_1_page_table.table[stack_vpn].prot = PROT_READ | PROT_WRITE;
    region_1_page_table.table[stack_vpn].pfn = free_frame;
    mem_frames.bit_arr[free_frame] = 1; // set that frame to used

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
}

//-------------------------- init_region1_page_table -----------------------------
// Description: Initializes a Region 1 Page Table and marks everything as invalid.
// Inputs:      None.
// Outputs:     An user page table is initilaized and marked as invalid.
//              That user page table is returned.
//--------------------------------------------------------------------------------
page_table_t init_region1_page_table()
{
    page_table_t temp_page_table;
    unsigned int page_id = 0;
    // Set up a Region 1 page table.
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        TracePrintf(0, "\n-------------------- %x ----------------------\n", i);
        page_id = i / PAGESIZE;
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        temp_page_table.table[page_id] = page_table_entry;
    }
    TracePrintf(0, "\n-------------------- Return ----------------------\n");
    return temp_page_table;
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
    pte_t page2;

    // Find free frames in physical memory
    u_long pfn1 = first_free_frame_idx();
    u_long pfn2 = first_free_frame_idx();

    // Mark them as used
    mem_frames.bit_arr[pfn1] = 1;
    mem_frames.bit_arr[pfn2] = 1;

    // Initialize the page table entries as valid
    page0.pfn = pfn1;
    page0.valid = 1;
    page0.prot = PROT_READ | PROT_WRITE;
    page2.pfn = pfn2;
    page2.valid = 1;
    page2.prot = PROT_READ | PROT_WRITE;

    // Copy the page table entries into the kernel stack
    temp_stack.table[0] = page0;
    temp_stack.table[1] = page2;

    return temp_stack;
}

//-------------------------------- init_process ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void init_process(UserContext *user_context)
{
    // FREE PAGE TABLE WHEN RETIRING PROCESS
    page_table_t *temp_page_table = malloc(sizeof(page_table_t));
    TracePrintf(0, "-------------------------- init pt addr %p-----------------", temp_page_table);
    unsigned int page_id = 0;
    // Set up a Region 1 page table.
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {

        TracePrintf(0, "\n-------------------- %x ----------------------\n", i);
        page_id = i / PAGESIZE;
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        temp_page_table->table[page_id] = page_table_entry;
    }

    init_pcb.memory_context.user_page_table = *temp_page_table;

    // Get a PID for the process
    init_pcb.pid = helper_new_pid(init_pcb.memory_context.user_page_table.table);

    // Set up a Kernel stack table.
    init_pcb.memory_context.kernel_stack = new_kernel_stack();

    // Indicate the virtual memory base address of the region 1 page table.
    WriteRegister(REG_PTBR1, (unsigned int)init_pcb.memory_context.user_page_table.table);

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, N_R1_PTE_ENTRIES);

    // Make a copy of the user context int the init PCB
    init_pcb.user_context = *user_context;
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

    TracePrintf(0, "\n------------------------ data start: %p----------------------\n", _kernel_data_start);
    TracePrintf(0, "\n------------------------ data end: %p----------------------\n", _kernel_data_end);

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
    // Set up a Region 1 page table.
    for (unsigned int i = VMEM_1_BASE; i < VMEM_1_LIMIT; i += PAGESIZE)
    {
        page_id = i / PAGESIZE;
        pte_t page_table_entry;

        // Mark everything as invalid.
        page_table_entry.valid = 0;
        page_table_entry.prot = 0;
        page_table_entry.pfn = 0;
        region_1_page_table.table[page_id] = page_table_entry;
    }

    // Indicate the virtual memory base address of the region 1 page table.
    WriteRegister(REG_PTBR1, (unsigned int)region_1_page_table.table);

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
    init_process(uctxt);
    TracePrintf(0, "\n--------------- LEFT CREATE INIT PROC ---------------\n");

    // Clone idle into init
    TracePrintf(0, "\n--------------- About to Clone IDLE into INIT ---------------\n");
    int status = KernelContextSwitch(KCCopy, (void *)curr_pcb, NULL);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
    }
    TracePrintf(0, "\n--------------- Back from the clone! Am I IDLE or INIT? ---------------\n");

    // if (cmd_args[0] == NULL)
    // {
    //     // Load init executable into the region 1 page table of the init process
    //     TracePrintf(0, "\n--------------- About to Load INIT ---------------\n");
    //     status = LoadProgram("init", NULL, &init_pcb);
    //     if (status != 0)
    //     {
    //         TracePrintf(0, "\n--------------- Load Program Failed ---------------\n");
    //     }
    //     TracePrintf(0, "\n--------------- Back from Loading INIT? ---------------\n");
    // }

    // Load init executable into the region 1 page table of the init process
    TracePrintf(0, "\n--------------- About to Load INIT ---------------\n");
    status = LoadProgram(cmd_args[0], cmd_args, &init_pcb);

    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Load Program Failed ---------------\n");
    }
    TracePrintf(0, "\n--------------- Back from Loading INIT? ---------------\n");

    TracePrintf(0, "\n--------------- Leaving KernelStart ---------------\n");
}