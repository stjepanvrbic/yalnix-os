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
        new_pcb->parent_pcb_p = curr_pcb;

        // If we are the parent return the PID of the newborn child.
        return new_pcb->pid;
    }

    // If we are the Child, return 0.
    return SUCCESS;
}

int KernelExec(char *filename, char **argvec)
{
    int status = LoadProgram(filename, argvec, curr_pcb);
    if (status != 0)
    {
        TracePrintf(0, "\n--------------- Load Program Failed ---------------\n");
        return ERROR;
    }
}

void KernelExit(int status)
{
    /**
     * Save status value
     * Free all the resources used by that process
     * When parent exits, their children keep running.
     * When orphans exits, no need to save or report status.
     */

    // int status;
    // pcb_t *parent;

    // // // If the initial process exists, halt the system
    // if (curr_pcb->pid == idle_pcb.pid)
    // {
    //     Halt();
    // }

    // // If the parent is alive
    // if (curr_pcb->parent_pcb_p != NULL)
    // {
    //     parent = (pcb_t *)curr_pcb->parent_pcb_p;

    //     // Save the exit code
    //     curr_pcb->exit_code = status;

    //     // Free the resources used by the process
    //     qclose(curr_pcb->children);
    //     qclose(curr_pcb->deceased_children);

    //     // Pop pcb from queue of children
    //     pcb_t *next_pcb = (pcb_t *)qremove(parent->children, );
    //     // If queue is empty, error
    //     if (next_pcb == NULL)
    //     {
    //         return ERROR;
    //     }

    //     // Add pcb to deceased children queue
    //     status = (int)qput(parent->deceased_children, (void *)curr_pcb);
    //     if (status != 0)
    //     {
    //         TracePrintf(0, "\n--------------- ERROR : Adding pcb to queue FAILED ---------------\n");
    //         return ERROR;
    //     }
    // }

    // // If the process has childs

    // exit(SUCCESS);
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
    TracePrintf(0, "--------------Calling KernelBrk with addr: %p----------------", addr);
    TracePrintf(0, "\n---------- Current kernel brk %p-----------\n", curr_pcb->memory_context.brk);
    void *new_brk = (void *)UP_TO_PAGE(addr);

    void *lower_bound = (void *)VMEM_1_BASE; // QUESTION: NOT SURE WHAT TO PUT AS THE LOWER BOUND, PUTTING VMEM_1_BASE FOR NOW.
    void *upper_bound = (void *)DOWN_TO_PAGE(curr_pcb->memory_context.region_1_sp);
    int8_t isWithinBound = (lower_bound <= new_brk && new_brk <= upper_bound);
    if (!(isWithinBound))
    {
        TracePrintf(0, "\n---------- BRK ERRROR -----------\n");
        return ERROR;
    }

    curr_pcb->memory_context.brk = new_brk;
    TracePrintf(0, "\n---------- New kernel brk %p-----------\n", curr_pcb->memory_context.brk);
    return 0;
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