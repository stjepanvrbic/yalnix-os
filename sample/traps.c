/* traps.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 *
 * Description: Implements the trap handlers.
 *
 */

#include "../include/traps.h"
#include "../include/kernel_context.h"
#include "../include/process_coord_syscalls.h"
#include "../include/pcb.h"
#include "../include/queue.h"
#include "../include/hardware.h"

#include <unistd.h>
#include <yalnix.h>

void trap_kernel_handler(UserContext *user_context)
{
    TracePrintf(0, "\n------------ kernel trap triggered ----------------\n");

    // Get the trap code from the user_context
    int code = user_context->code;
    int addr, clock_ticks;
    char *filename;
    char **argvec;

    // Switch statement to invoke the correct syscall wrapper.
    switch (code)
    {
    case YALNIX_FORK:
        KernelFork();
        break;

    case YALNIX_EXEC:
        filename = (char *)user_context->regs[0];
        argvec = (char **)user_context->regs;
        KernelExec(filename, argvec);
        break;

    case YALNIX_EXIT:
        // KernelExit();
        break;

    case YALNIX_WAIT:
        // KernelWait();
        break;

    case YALNIX_GETPID:
        KernelGetPid();
        break;

    case YALNIX_BRK:
        addr = user_context->regs[0];
        KernelBrk((void *)addr);
        break;

    case YALNIX_DELAY:
        clock_ticks = user_context->regs[0];
        KernelDelay(clock_ticks);
        break;

    default:
        break;
    }

    // return
}

void trap_clock_handler(UserContext *user_context)
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

    curr_pcb->user_context = *user_context;

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
    TracePrintf(0, "\n--------------- Back from the switch! I AM pid: %d ---------------\n", curr_pcb->pid);
    // Set user context to new pcb context
    *user_context = curr_pcb->user_context;

    TracePrintf(0, "\n------------ leaving clock trap handler ----------------\n");
}

void trap_illegal_handler(UserContext *user_context)
{
    TracePrintf(0, "\n------------ illegal trap triggered ----------------\n");
    TracePrintf(0, "\n------------ Process calling Illegal Trap Handler pid: %d ----------------\n", curr_pcb->pid);

    int status = user_context->code;
    TracePrintf(0, "\n------------ Status Code when Exiting: %d ----------------\n", status);

    KernelExit(status);
}

void trap_memory_handler(UserContext *user_context)
{

    TracePrintf(0, "\n------------ memory trap triggered ----------------\n");
    TracePrintf(0, "\nOffending address: %p\n", user_context->addr);

    // Get the trap code from the user_context
    // Switch statement to invoke the correct syscall wrapper
    //      If trap is an exception to enlarge the current process's stack
    //          try to enlarge stack
    //          If successful
    //              return allowing the process to keep running
    //          Else
    //              invoke Exit() syscall to abort current process
    //      Else
    //          invoke Exit() syscall to abort current process

    // return
    TracePrintf(0, "\n------------ memory trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
}

void trap_math_handler(UserContext *user_context)
{
    TracePrintf(0, "\n------------ math trap triggered ----------------\n");
    TracePrintf(0, "\n------------ Process calling Math Trap Handler pid: %d ----------------\n", curr_pcb->pid);

    int status = user_context->code;
    TracePrintf(0, "\n------------ Status Code when Exiting: %d ----------------\n", status);

    KernelExit(status);
}

void trap_tty_receive_handler(UserContext *user_context)
{
    // Get the trap code from the user_context

    // Read the input from the terminal using TtyReceive while passing the terminal to read from.
    // If necessary
    //      buffer the input line for a subsequent TtyRead syscall by some user process ??

    // return
    TracePrintf(0, "\n------------ tty receive trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
}

void trap_tty_transmit_handler(UserContext *user_context)
{
    // Get the trap code from the user_context

    // Run the blocked process that started the terminal output
    // If any
    //      start the next terminal output on this terminal

    // return
    TracePrintf(0, "\n------------ tty transmit trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
}

void trap_disk_handler(UserContext *user_context)
{
    TracePrintf(0, "\n------------ disk trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");

    return;
}