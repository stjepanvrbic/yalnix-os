/* kernel_context.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Feb 12
 * Version: 1.0
 *
 * Description: Implements the kernel switch and kernel copy functions.
 */

#include "../include/kernel_context.h"
#include "../include/pcb.h"
#include "../include/utils.h"
#include <ylib.h>

/* The kernel stack initially maps 1 to 1 to phtsical addresses
In the region 0 page table, the vpn correspond 1 to 1 to the psiginfo
When we want to change context, we will loop through the vpns in the region 0 page table corresponding
to the kernel stack
And we will change the pfns to the pfns of the kernel stack table stored in the pcb of the
process we are switching to. */
KernelContext *KCSwitch(KernelContext *kc_in,
                        void *curr_pcb_p,
                        void *next_pcb_p)
{
    pcb_t *p_curr_pcb = (pcb_t *)curr_pcb_p;
    pcb_t *p_next_pcb = (pcb_t *)next_pcb_p;

    // Save the current process kernel context
    p_curr_pcb->kernel_context = *kc_in;

    void *kernel_stack_p0 = (void *)(VMEM_0_LIMIT - PAGESIZE);
    void *kernel_stack_p1 = (void *)(VMEM_0_LIMIT - (2 * PAGESIZE));

    unsigned int kernel_stack_vpn0 = (unsigned int)kernel_stack_p0 >> PAGESHIFT;
    unsigned int kernel_stack_vpn1 = (unsigned int)kernel_stack_p1 >> PAGESHIFT;
    // Check if these two are swapped

    // Save the current process kernel stack in its PCB
    p_curr_pcb->memory_context.kernel_stack.table[0] = kernel_page_table.table[kernel_stack_vpn0];
    p_curr_pcb->memory_context.kernel_stack.table[1] = kernel_page_table.table[kernel_stack_vpn1];

    // Map the kernel page table to the new process kernel stack
    kernel_page_table.table[kernel_stack_vpn0] = p_next_pcb->memory_context.kernel_stack.table[0];
    kernel_page_table.table[kernel_stack_vpn1] = p_next_pcb->memory_context.kernel_stack.table[1];

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);

    // Return the next process kernel context
    return &p_next_pcb->kernel_context;
}

KernelContext *KCCopy(KernelContext *kc_in,
                      void *new_pcb_p,
                      void *not_used)
{
    // Copy contents of the kernel stack into the new process kernel stack
    pcb_t *new_pcb = (pcb_t *)new_pcb_p;

    // Copy kc_in into the new_pcb_p
    new_pcb->kernel_context = *kc_in;

    // Get two new pte_e for the new process
    kernel_stack_t new_k_stack = new_kernel_stack();
    new_pcb->memory_context.kernel_stack = new_k_stack;

    // Get pointers to the two kernel stack pages
    void *old_stack_page0_p = (void *)(VMEM_0_LIMIT - PAGESIZE);
    void *old_stack_page1_p = (void *)(VMEM_0_LIMIT - 2 * PAGESIZE);

    // Get pointers to the two pages below the kernel stack
    void *new_stack_page0_p = (void *)(VMEM_0_LIMIT - 3 * PAGESIZE);
    void *new_stack_page1_p = (void *)(VMEM_0_LIMIT - 4 * PAGESIZE);

    // Go from addresses to VPNs
    unsigned int new_stack_vpn0 = (unsigned int)new_stack_page0_p >> PAGESHIFT;
    unsigned int new_stack_vpn1 = (unsigned int)new_stack_page1_p >> PAGESHIFT;

    // Put the new kernel stack right below the current one in virtual memory
    kernel_page_table.table[new_stack_vpn0] = new_k_stack.table[0];
    kernel_page_table.table[new_stack_vpn1] = new_k_stack.table[1];

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);

    // Copy the contents of the old kernel stack to the new one
    memcpy(new_stack_page1_p, old_stack_page1_p, PAGESIZE);
    memcpy(new_stack_page0_p, old_stack_page0_p, PAGESIZE);

    // Mark the new kernel kernel stack as invalid again
    // Mark the PFNs as available in the bit array
    kernel_page_table.table[new_stack_vpn0].valid = 0;
    kernel_page_table.table[new_stack_vpn0].pfn = 0;

    kernel_page_table.table[new_stack_vpn1].valid = 0;
    kernel_page_table.table[new_stack_vpn1].pfn = 0;

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);

    // Return kc_in
    return kc_in;
}

// Stops the current process, adds it to the end of the ready queue,
// and switches to the next ready process. Default process is idle if no
// other process is in the ready queue.
void switch_to_next_ready_process()
{
    // If current pcb is not idle nor blocked nor defunct, add it back to the end of the ready queue.
    int status;
    bool isBlocked = qsearch(blocked_queue, (void *)&search_pcb, (void *)&curr_pcb->pid) != NULL;
    bool isDefunct = qsearch(defunct_queue, (void *)&search_pcb, (void *)&curr_pcb->pid) != NULL;
    if (curr_pcb->pid != idle_pcb.pid && !isBlocked && !isDefunct)
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
    if (curr_pcb->pid == idle_pcb.pid && next_pcb == NULL)
    {
        return;
    }

    // If current pcb isn't idle and the ready is empty, next pcb is idle.
    if (curr_pcb->pid != idle_pcb.pid && next_pcb == NULL)
    {
        next_pcb = &idle_pcb;
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