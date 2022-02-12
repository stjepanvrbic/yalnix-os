/* kernel_context.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Feb 12
 * Version: 1.0
 *
 * Description: Implements the kernel switch and kernel copy functions.
 */

#include "include/hardware.h"
#include "../include/pcb.h"

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
    /* Save current process kernel stack
    Add current process PCB to blocked queue
    Change kernel stack page table entries for the next process
    Return the PCB for the next process */
}

void copy_page_contents(u_int8_t *page1, u_int8_t *page2)
{
    for (int i = 0; i < PAGESIZE; i++)
    {
        page2[0] = page1[0];
    }
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
    ((pcb_t *)new_pcb_p)->memory_context.kernel_stack = new_k_stack;

    void *old_stack_page0_p = VMEM_0_LIMIT - PAGESIZE;
    void *old_stack_page1_p = VMEM_0_LIMIT - 2 * PAGESIZE;
    void *new_stack_page0_p = VMEM_0_LIMIT - 3 * PAGESIZE;
    void *new_stack_page1_p = VMEM_0_LIMIT - 4 * PAGESIZE;

    unsigned int new_stack_vpn0 = (unsigned int)new_stack_page0_p & PAGEMASK;
    unsigned int new_stack_vpn1 = (unsigned int)new_stack_page1_p & PAGEMASK;

    // Put the new kernel stack right below the current one in virtual memory
    extern kernel_page_table[new_stack_vpn0] = new_k_stack.table[0];
    extern kernel_page_table[(VMEM_0_LIMIT - (PAGESIZE * 4)) & PAGEMASK] = new_k_stack.table[1];
    // Copy the contents of the old kernel stack to the new one

    // Mark the new kernel kernel stack as invalid again

    // Return kc_in
    return kc_in;
}