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

#include "hardware.h"
#include "utils.h"

typedef struct page_table
{
    pte_t table[VMEM_1_SIZE / PAGESIZE]; // Page table for the process
} page_table_t;

typedef struct kernel_page_table
{
    pte_t table[VMEM_0_SIZE / PAGESIZE]; // Kernel Page table for the process
} kernel_page_table_t;

typedef struct memblock
{
    void *brk;       // Pointer to the brk in Region 1
    void *kernel_sp; // Pointer to the top of the kernel stack

    page_table_t user_page_table;    // Page table for the Region 1 process
    kernel_page_table_t kernel_spage_table; // Page table for the kernel stack

} memblock_t;

typedef struct pcb
{
    int pid;       // The process id
    void *p_parent_proc; // Pointer to the parent process PCB

    UserContext user_context;     // UserContext information
    KernelContext kernel_context; // KernelContext information

    queue_t children;          // Keeping track of the child processes
    queue_t deceased_children; // Keeping track of the dead children

    int exit_code;   // Exit code of the process
    int exit_signal; // The signal that ended the process

    void *p_prev; // Pointer to the previous PCB element in the ready/blocked queue
    void *p_next; // Pointer to the next PCB element in the ready/blocked queue

    memblock_t memory_context; // Memory context block for the process

} pcb_t;