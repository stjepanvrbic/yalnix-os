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

void trap_kernel_handler(UserContext *user_context)
{
    // Get the trap code from the user_context
    // Switch statement to invoke the correct syscall wrapper

    // return
    TracePrintf(0, "\n------------ kernel trap triggered ----------------\n");
    TracePrintf(0, "\nSyscall code : %x\n", user_context->code);
}

void trap_clock_handler(UserContext *user_context)
{
    // If there other processes in the ready queue
    //      (round-robin process scheduling)
    //      perform context switch to the first process in the ready queue
    // Else
    //      dispatch idle
    TracePrintf(0, "\n------------ clock trap triggered ----------------\n");
}

void trap_illegal_handler(UserContext *user_context)
{
    // Invoke Exit() syscall to abort current process
    TracePrintf(0, "\n------------ illegal trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
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
    // Invoke Exit() syscall to abort current process
    TracePrintf(0, "\n------------ math trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
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
    // return
    TracePrintf(0, "\n------------ disk trap triggered ----------------\n");
    TracePrintf(0, "\nTHIS TRAP IS NOT YET HANDLED\n");
}