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
    switch_to_next_ready_process();
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