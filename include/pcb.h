#pragma once
/* pcb.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Jan 31
 * Version: 1.0
 *
 * Description: Declares the data structures used for the PCBs.
 *
 *
 */
#ifndef _pcb_h
#define _pcb_h
#include "hardware.h"
#include "queue.h"

typedef struct page_table
{
    pte_t table[VMEM_1_SIZE / PAGESIZE]; // Page table for the process
} page_table_t;

typedef struct kernel_page_table
{
    pte_t table[VMEM_0_SIZE / PAGESIZE]; // Kernel Page table for the process
} kernel_page_table_t;

typedef struct kernel_stack
{
    pte_t table[KERNEL_STACK_MAXSIZE / PAGESIZE];
} kernel_stack_t;

typedef struct memblock
{
    void *brk;         // Pointer to the brk in Region 1
    void *region_1_sp; // Pointer to the bottom of the region 1 stack

    page_table_t *user_page_table; // Page table for the Region 1 process
    kernel_stack_t kernel_stack;   // Page table for the kernel stack

} memblock_t;

typedef struct pcb
{
    int pid;            // The process id
    void *parent_pcb_p; // Pointer to the parent process PCB

    UserContext user_context;     // UserContext information
    KernelContext kernel_context; // KernelContext information

    queue_t *children;          // Keeping track of the child processes
    queue_t *deceased_children; // Keeping track of the dead children

    int exit_code;   // Exit code of the process
    int exit_signal; // The signal that ended the process

    void *p_prev; // Pointer to the previous PCB element in the ready/blocked queue
    void *p_next; // Pointer to the next PCB element in the ready/blocked queue

    memblock_t memory_context; // Memory context block for the process

} pcb_t;

kernel_stack_t new_kernel_stack();
int first_free_frame_idx();
void create_process(UserContext *user_context, pcb_t *new_pcb);

// Set up the Kernel Page Table.
extern kernel_page_table_t kernel_page_table;

// Set up the Region 1 Page Table.
extern page_table_t *region_1_page_table;

// Globals to keep track of processes.
extern pcb_t *curr_pcb;
extern pcb_t idle_pcb;
#endif