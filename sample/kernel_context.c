
#include "include/hardware.h"

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

KernelContext *KCCopy(KernelContext *kc_in,
                      void *new_pcb_p,
                      void *not_used)
{
    /* Copy kc_in into the new_pcb_p
    Copy contents of the kernel stack into the new process kernel stack
    Return kc_in */
}