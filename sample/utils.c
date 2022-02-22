/* utils.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 *
 * Description: Implements the process coordination syscalls handlers.
 *
 */

#include <ylib.h>
#include "../include/queue.h"
#include "../include/pcb.h"
#include "../include/utils.h"

void remove_parent(void *elementp)
{
    pcb_t *child_pcb = (pcb_t *)elementp;

    child_pcb->parent_pcb_p = NULL;
}

bool search_pcb(void *elementp, const void *keyp)
{
    pcb_t *element = (pcb_t *)elementp;
    int *key_pid = (int *)keyp;

    if (element->pid == *key_pid)
    {
        return true;
    }
    return false;
}


// Stops the current process, adds it to the end of the ready queue,
// and switches to the next ready process. Default process is idle if no
// other process is in the ready queue.
void switch_to_next_ready_process()
{
    // If current pcb is not idle, add it back to the end of the ready queue.
    int status;
    if (curr_pcb->pid != idle_pcb.pid)
    {
        status = (int)qput(ready_queue, (void *)curr_pcb);
        // If can't put in the queue, keep running the current process.
        if (status != 0)
        {
            TracePrintf(0, "\n--------------- ERROR : Adding pcb to queue FAILED ---------------\n");
            return;
        }
    }

    // Get first process from the ready queue
    pcb_t *next_pcb = (pcb_t *)qget(ready_queue);
    // If we were idle and the queue is empty, keep running idle.
    if (next_pcb == NULL)
    {
        return;
    }

    curr_pcb->user_context = *curr_uctxt;

    // Write the page table for the next process to the register
    WriteRegister(REG_PTBR1, (unsigned int)next_pcb->memory_context.user_page_table->table);

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, N_R1_PTE_ENTRIES);

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    pcb_t *prev_pcb = curr_pcb;
    curr_pcb = next_pcb;

    // Kernel Context Switch from previous to next pcb.
    TracePrintf(0, "\n------------ About to Switch from pid: %d to pid: %d ----------------\n", prev_pcb->pid, next_pcb->pid);
    status = KernelContextSwitch(KCSwitch, (void *)prev_pcb, (void *)next_pcb);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
    }
    TracePrintf(0, "\n--------------- Back from the switch! I am pid: %d ---------------\n", curr_pcb->pid);
    // Set user context to new pcb context
    *curr_uctxt = curr_pcb->user_context;
}