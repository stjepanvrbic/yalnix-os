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
    void *kernel_stack_p1 = (void *)(VMEM_0_LIMIT - 2 * PAGESIZE);

    unsigned int kernel_stack_vpn0 = (unsigned int)kernel_stack_p0 & PAGEMASK;
    unsigned int kernel_stack_vpn1 = (unsigned int)kernel_stack_p1 & PAGEMASK;

    // Save the current process kernel stack in its PCB
    p_curr_pcb->memory_context.kernel_stack.table[0] = kernel_page_table.table[kernel_stack_vpn0];
    p_curr_pcb->memory_context.kernel_stack.table[1] = kernel_page_table.table[kernel_stack_vpn1];

    // Map the kernel page table to the new process kernel stack
    kernel_page_table.table[kernel_stack_vpn0] = p_next_pcb->memory_context.kernel_stack.table[0];
    kernel_page_table.table[kernel_stack_vpn1] = p_next_pcb->memory_context.kernel_stack.table[1];

    // Return the next process kernel context
    return &p_next_pcb->kernel_context;
}

void copy_page_contents(u_int8_t *dest, u_int8_t *src)
{
    for (int i = 0; i < PAGESIZE; i++)
    {
        src[0] = dest[0];
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
    new_pcb->memory_context.kernel_stack = new_k_stack;

    void *old_stack_page0_p = (void *)(VMEM_0_LIMIT - PAGESIZE);
    void *old_stack_page1_p = (void *)(VMEM_0_LIMIT - 2 * PAGESIZE);
    void *new_stack_page0_p = (void *)(VMEM_0_LIMIT - 3 * PAGESIZE);
    void *new_stack_page1_p = (void *)(VMEM_0_LIMIT - 4 * PAGESIZE);

    unsigned int new_stack_vpn0 = (unsigned int)new_stack_page0_p & PAGEMASK;
    unsigned int new_stack_vpn1 = (unsigned int)new_stack_page1_p & PAGEMASK;

    // Put the new kernel stack right below the current one in virtual memory
    kernel_page_table.table[new_stack_vpn0] = new_k_stack.table[0];
    kernel_page_table.table[new_stack_vpn1] = new_k_stack.table[1];

    // Copy the contents of the old kernel stack to the new one
    copy_page_contents((u_int8_t *)old_stack_page1_p, (u_int8_t *)new_stack_page1_p);

    // Mark the new kernel kernel stack as invalid again
    // Mark the PFNs are available in the bit array
    kernel_page_table.table[new_stack_vpn0].valid = 0;
    mem_frames.bit_arr[kernel_page_table.table[new_stack_vpn0].pfn] = 0;
    kernel_page_table.table[new_stack_vpn0].pfn = 0;

    kernel_page_table.table[new_stack_vpn1].valid = 0;
    mem_frames.bit_arr[kernel_page_table.table[new_stack_vpn1].pfn] = 0;
    kernel_page_table.table[new_stack_vpn1].pfn = 0;

    // Return kc_in
    return kc_in;
}