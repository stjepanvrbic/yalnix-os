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
#include <unistd.h>
#include <yalnix.h>

void trap_kernel_handler(UserContext *user_context)
{
    TracePrintf(0, "\n------------ kernel trap triggered ----------------\n");

    // Get the trap code from the user_context
    int code = user_context->code;
    int addr, clock_ticks;

    // Switch statement to invoke the correct syscall wrapper
    switch (code)
    {
    case YALNIX_FORK:
        // KernelFork();
        break;

    case YALNIX_EXEC:
        // user_context->code;
        // KernelExec();
        break;

    case YALNIX_EXIT:
        // KernelFork();
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
    // If there other processes in the ready queue
    //      (round-robin process scheduling)
    //      perform context switch to the first process in the ready queue
    // Else
    //      dispatch idle

    // Check which process is running
    // If idle currently running
    if (curr_pcb->pid == idle_pcb.pid)
    {
        TracePrintf(0, "\n------------ I AM IDLE about to switch to INIT ----------------\n");
        // Save user context
        idle_pcb.user_context = *user_context;

        // Indicate the virtual memory base address of the region 1 page table.
        WriteRegister(REG_PTBR1, (unsigned int)init_pcb.memory_context.user_page_table->table);
    }
    // If init currently running
    else
    {
        TracePrintf(0, "\n------------ I AM INIT about to switch to IDLE ----------------\n");
        // Save user context
        init_pcb.user_context = *user_context;

        // Indicate the virtual memory base address of the region 1 page table.
        WriteRegister(REG_PTBR1, (unsigned int)idle_pcb.memory_context.user_page_table->table);
    }

    // Indicate the number of page table entries in the region 1 page table.
    WriteRegister(REG_PTLR1, N_R1_PTE_ENTRIES);

    // Flush the TLB
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    // Check which process is running
    // If idle currently running
    if (curr_pcb->pid == idle_pcb.pid)
    {
        // Set the new current pcb
        curr_pcb = &init_pcb;
        // Switch from idle to init
        TracePrintf(0, "\n------------ I AM IDLE about to switch to INIT ----------------\n");
        int status = KernelContextSwitch(KCSwitch, (void *)&idle_pcb, (void *)&init_pcb);
        if (status != 0)
        {
            TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
        }
        TracePrintf(0, "\n--------------- Back from the switch! I AM IDLE ---------------\n");
        // Set user context to new pcb context
        *user_context = idle_pcb.user_context;
    }
    // If init currently running
    else
    {
        // Set the new current pcb
        curr_pcb = &idle_pcb;
        // Switch from init to idel
        TracePrintf(0, "\n--------------- I AM INIT about to switch to IDLE ---------------\n");
        int status = KernelContextSwitch(KCSwitch, (void *)&init_pcb, (void *)&idle_pcb);
        if (status != 0)
        {
            TracePrintf(0, "\n--------------- Kernel Context Switch Failed ---------------\n");
        }
        TracePrintf(0, "\n--------------- Back from the switch! I AM INIT ---------------\n");
        // Set user context to new pcb context
        *user_context = init_pcb.user_context;
    }

    TracePrintf(0, "\n------------ leaving clock trap handler ----------------\n");
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