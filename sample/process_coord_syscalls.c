/* process_coord_syscalls.c ---
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
#include "../include/pcb.h"
#include "../include/hardware.h"
#include "../include/process_coord_syscalls.h"
#include "../include/utils.h"
#include "../include/load_program.h"
#include "../include/kernel_context.h"

int KernelFork(void)
{
    TracePrintf(0, "\n------------ Fork is triggered ----------------\n");
    // Create New Process PCB.
    pcb_t *new_pcb = malloc(sizeof(pcb_t));
    create_process(curr_uctxt, new_pcb);

    // Add new PCB to the ready queue.
    int status = (int)qput(ready_queue, (void *)new_pcb);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- ERROR : Adding pcb to queue FAILED ---------------\n");
        return ERROR;
    }

    TracePrintf(0, "\n------------ About to Create Region 1 Page Table ----------------\n");
    // Copy the region 1 page table
    //  Make an empty page table new_pt
    page_table_t *new_pt = new_pcb->memory_context.user_page_table;

    TracePrintf(0, "\n------------ About to Initialize Region 1 Page Table ----------------\n");
    // Loop through the current region 1 page table curr_pt
    for (int vpn = 0; vpn < MAX_PT_LEN; vpn++)
    {
        if (curr_pcb->memory_context.user_page_table->table[vpn].valid == 0)
        {
            continue;
        }

        // Get a new free frame (physical)
        int free_idx = first_free_frame_idx();

        // Map the page below the kernel stack to it
        void *page_below_kstack_addr = (void *)(VMEM_0_LIMIT - (3 * PAGESIZE));
        void *region_1_addr = (void *)((vpn + MAX_PT_LEN) << PAGESHIFT);
        int page_below_kstack = ((int)page_below_kstack_addr) >> PAGESHIFT;

        kernel_page_table.table[page_below_kstack].valid = 1;
        kernel_page_table.table[page_below_kstack].prot = PROT_READ | PROT_WRITE;
        kernel_page_table.table[page_below_kstack].pfn = free_idx;

        // Flush the TLB
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_ALL);

        // Memcpy from address of curr_pt[vpn] to address of kernel_pt[MAX-3]
        memcpy(page_below_kstack_addr, region_1_addr, PAGESIZE);

        // Put the PTE at kernel_pt[MAX-3] into new_pt[vpn]
        new_pt->table[vpn] = kernel_page_table.table[page_below_kstack];

        // Set the permissions to be the same as the region 1 pte
        new_pt->table[vpn].valid = curr_pcb->memory_context.user_page_table->table[vpn].valid;
        new_pt->table[vpn].prot = curr_pcb->memory_context.user_page_table->table[vpn].prot;

        // Set the page table entry in the kernel as invalid and free the frame
        kernel_page_table.table[page_below_kstack].valid = 0;
        kernel_page_table.table[page_below_kstack].prot = 0;
        kernel_page_table.table[page_below_kstack].pfn = 0;

        // Flush the TLB
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_ALL);
    }

    // Copy current context to new PCB.
    status = KernelContextSwitch(KCCopy, (void *)new_pcb, NULL); // NOTE: both current and next processes will read the rest of the function.
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
        return ERROR;
    }

    // If we are the Parent.
    if (curr_pcb->pid != new_pcb->pid)
    {
        // Add the child process to the queue of children processes.
        int status = (int)qput(curr_pcb->children, (void *)new_pcb);
        if (status != 0)
        {
            TracePrintf(0, "\n--------------- ERROR : Adding new pcb to queue FAILED ---------------\n");
            return ERROR;
        }
        // If we are the parent make the parent pcb of the child process point to me
        new_pcb->parent_pcb_p = (void *)curr_pcb;

        TracePrintf(0, "\n------------ I'M THE PARENT -> About to leave fork ----------------\n");

        // If we are the parent return the PID of the newborn child.
        return new_pcb->pid;
    }
    else
    {
        TracePrintf(0, "\n------------ I'M THE CHILD my PID is: %d ----------------\n", curr_pcb->pid);
        TracePrintf(0, "\n------------ I'M THE CHILD -> About to leave fork ----------------\n");
    }

    TracePrintf(0, "\n------------ Returning to the child ----------------\n");
    // If we are the Child, return 0.
    return SUCCESS;
}

int KernelExec(char *filename, char **argvec)
{
    TracePrintf(0, "\n------------ Exec is triggered ----------------\n");
    int status = LoadProgram(filename, argvec, curr_pcb);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Load Program Failed ---------------\n");
        return ERROR;
    }
    TracePrintf(0, "\n------------ Leaving Exec ----------------\n");
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_ALL);
}

void KernelExit(int status)
{
    /**
     * Save status value
     * Free all the resources used by that process
     * When parent exits, their children keep running.
     * When orphans exits, no need to save or report status.
     */

    TracePrintf(0, "\n------------ Exit Triggered ----------------\n");

    int func_status;
    pcb_t *parent = (pcb_t *)curr_pcb->parent_pcb_p;

    TracePrintf(0, "\n------------ Parent pid: %d ----------------\n", parent->pid);

    // If the initial process exists, halt the system
    if (curr_pcb->pid == idle_pcb.pid)
    {
        Halt();
    }

    // Set the current process childs parent to Null
    qapply(curr_pcb->children, &remove_parent);

    // Check if the parent exists
    pcb_t *in_defunct_queue = NULL;
    if (parent != NULL)
    {
        // Check if the parent is in the defunct queue
        pcb_t *in_defunct_queue = (pcb_t *)qsearch(defunct_queue, (void *)&search_pcb, (void *)&parent->pid);
    }

    // If the parent is alive
    if (in_defunct_queue == NULL) // Check if the parent is in the defunct queue
    {
        // Save the exit code
        curr_pcb->exit_code = status;

        // Free the resources used by the process
        qclose(curr_pcb->children);
        qclose(curr_pcb->deceased_children);
        free(curr_pcb->memory_context.user_page_table);

        // Pop pcb from queue of children
        pcb_t *found_pcb = (pcb_t *)qremove(parent->children, (void *)&search_pcb, (void *)&curr_pcb->pid);
        // If queue is empty, error
        if (found_pcb == NULL)
        {
            TracePrintf(0, "\n--------------- ERROR : Removing PCB from the parent's children FAILED ---------------\n");
        }

        // Add pcb to deceased children queue
        func_status = (int)qput(parent->deceased_children, (void *)curr_pcb);
        if (func_status != 0)
        {
            TracePrintf(0, "\n--------------- ERROR : Adding pcb to deceased children queue FAILED ---------------\n");
        }

        // Check if parent is in blocked queue
        // If so remove it from blocked queue and add it to the ready queue
        pcb_t *in_blocked_queue = (pcb_t *)qremove(blocked_queue, (void *)&search_pcb, (void *)&parent->pid);
        if (in_blocked_queue != NULL)
        {
            func_status = (int)qput(ready_queue, (void *)in_blocked_queue);
            if (func_status != 0)
            {
                TracePrintf(0, "\n--------------- ERROR : Adding pcb to ready queue FAILED ---------------\n");
            }
        }
    }

    // Add dead process to defunct queue
    func_status = (int)qput(defunct_queue, (void *)curr_pcb);
    if (func_status != 0)
    {
        TracePrintf(0, "\n--------------- ERROR : Adding pcb to ready queue FAILED ---------------\n");
    }

    switch_to_next_ready_process();

    return;
}

int KernelWait(int *status_ptr)
{
    /**
     * Collects process ID and exit status returned by a child of the program calling.
     * If caller has an exited child whose info hasnt been collected yet by Wait, return it directly.
     * If there are no child processes to wait for, return with ERROR.
     * If there are children processes not terminated, block and wait for exit info to return.
     * On success, the pid of the child is returned and status ptr is written to.
     */

    int func_status;

    // If the process has no children, or defunct children, return error
    if (qisempty(curr_pcb->children) && qisempty(curr_pcb->deceased_children))
    {
        TracePrintf(0, "\n--------------- ERROR : process has no children, or defunct children ---------------\n");
        return ERROR;
    }
    // If the process has defunct children, we can just return the pid of the first one
    else if (!qisempty(curr_pcb->deceased_children))
    {
        TracePrintf(0, "\n--------------- Defunct children case ---------------\n");
        pcb_t *first_child = (pcb_t *)qget(curr_pcb->deceased_children);
        if (first_child == NULL)
        {
            TracePrintf(0, "\n--------------- ERROR : Deceased Children queue is  EMPTY ---------------\n");
            return ERROR;
        }
        *status_ptr = first_child->exit_code;
        return first_child->pid;
    }
    // If the process has no defunct children, but has active children, put the process
    // In the blocked queue, and switch to the next ready process
    else if (!qisempty(curr_pcb->children))
    {
        TracePrintf(0, "\n--------------- No defunct children, but running children case ---------------\n");
        func_status = (int)qput(blocked_queue, curr_pcb); // Add dead process to defunct queue
        if (func_status != 0)
        {
            TracePrintf(0, "\n--------------- ERROR : Adding pcb to blocked queue FAILED ---------------\n");
            return ERROR;
        }
        // Switch to the next process in the ready queue
        switch_to_next_ready_process();

        // Set the status pointer and return the pid of the child
        pcb_t *first_child = (pcb_t *)qget(curr_pcb->deceased_children);
        if (first_child == NULL)
        {
            TracePrintf(0, "\n--------------- ERROR : Deceased Children queue is  EMPTY ---------------\n");
            return ERROR;
        }
        *status_ptr = first_child->exit_code;
        return first_child->pid;
    }
}

int KernelGetPid(void)
{
    // Returns the pid of the calling process
    return curr_pcb->pid;
}

// KernelLand handler for the Region 1 Brk syscall. DIFFERENT FROM SetKernelBrk.
int KernelBrk(void *addr)
{
    /**
     * Set the brk to addr (rounded up to the next multiple of PAGESIZE bytes).
     * Return 0 if succes.
     * If the addr is invalid or if not enough memory is available, the value ERROR is returned.
     */
    TracePrintf(0, "\n-------------- Calling KernelBrk with addr: %x ---------------\n", addr);
    TracePrintf(0, "\n-------------- Current kernel brk %x -------------------------\n", curr_pcb->memory_context.brk);
    void *new_brk = (void *)UP_TO_PAGE(addr);

    void *lower_bound = (void *)VMEM_1_BASE; // QUESTION: NOT SURE WHAT TO PUT AS THE LOWER BOUND, PUTTING VMEM_1_BASE FOR NOW.
    void *upper_bound = (void *)DOWN_TO_PAGE(curr_pcb->memory_context.region_1_sp);
    bool isWithinBound = ((lower_bound <= new_brk) && (new_brk <= upper_bound));

    TracePrintf(0, "\n-------------- Lower Bound: %x ----------------\n", (void *)VMEM_1_BASE);
    TracePrintf(0, "\n-------------- Upper Bound: %x ----------------\n", (void *)upper_bound);

    if (!isWithinBound)
    {
        TracePrintf(0, "\n---------- BRK ERRROR -----------\n");
        return ERROR;
    }

    void *old_brk = curr_pcb->memory_context.brk;
    curr_pcb->memory_context.brk = new_brk;

    // If the brk is being lowered
    if (new_brk < old_brk)
    {
        int low_vpn = (int)((int)new_brk >> PAGESHIFT) - MAX_PT_LEN;
        int high_vpn = (int)((int)old_brk >> PAGESHIFT) - MAX_PT_LEN;

        // Unmap the pages, and free the frames
        for (int vpn = low_vpn; vpn < high_vpn; vpn++)
        {
            curr_pcb->memory_context.user_page_table->table[vpn].valid = 0;
            curr_pcb->memory_context.user_page_table->table[vpn].prot = 0;
            mem_frames.bit_arr[curr_pcb->memory_context.user_page_table->table[vpn].pfn] = 0;
            curr_pcb->memory_context.user_page_table->table[vpn].pfn = 0;
        }
    }
    else
    {
        int low_vpn = (int)((int)old_brk >> PAGESHIFT) - (int)MAX_PT_LEN;
        int high_vpn = (int)((int)new_brk >> PAGESHIFT) - (int)MAX_PT_LEN;

        // If the brk is being raised
        // Mark the new entries as valid.
        for (int vpn = low_vpn; vpn < high_vpn; vpn++)
        {
            int free_frame = first_free_frame_idx();
            curr_pcb->memory_context.user_page_table->table[vpn].valid = 1;
            curr_pcb->memory_context.user_page_table->table[vpn].prot = PROT_READ | PROT_WRITE;
            curr_pcb->memory_context.user_page_table->table[vpn].pfn = free_frame;
        }
    }

    TracePrintf(0, "\n---------- New kernel brk %x-----------\n", curr_pcb->memory_context.brk);
    return SUCCESS;
}

int KernelDelay(int clock_ticks)
{
    // Check argumets
    if (clock_ticks < 0)
    {
        TracePrintf(0, "\n---------- ERROR KernelDelay: clock tick is negative: %d -----------\n", clock_ticks);
        return ERROR;
    }

    // Block until number of click ticks have passed
    while (clock_ticks > 0)
    {
        clock_ticks--;
    }

    return SUCCESS;
}