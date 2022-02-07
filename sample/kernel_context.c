
#include "include/hardware.h"

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