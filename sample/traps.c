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
    int addr, clock_ticks, status;
    char *filename;
    char **argvec;
    u_long response;

    // Switch statement to invoke the correct syscall wrapper.
    switch (code)
    {
    case YALNIX_FORK:
        TracePrintf(0, "\n------------ TRAP IN FORK CASE ----------------\n");
        response = KernelFork();
        user_context->regs[0] = response;
        break;

    case YALNIX_EXEC:
        TracePrintf(0, "\n------------ TRAP IN EXEC CASE ----------------\n");
        filename = (char *)user_context->regs[0];
        argvec = (char **)user_context->regs;
        response = KernelExec(filename, argvec);
        user_context->regs[0] = response;
        break;

    case YALNIX_EXIT:
        TracePrintf(0, "\n------------ TRAP IN EXIT CASE ----------------\n");
        status = (int)user_context->regs[0];
        TracePrintf(0, "\n------------ Exit Status: %d ----------------\n", status);
        KernelExit(status);
        TracePrintf(0, "\n------------ Done exiting ----------------\n", status);
        break;

    case YALNIX_WAIT:
        TracePrintf(0, "\n------------ TRAP IN WAIT CASE ----------------\n");
        // KernelWait();
        break;

    case YALNIX_GETPID:
        TracePrintf(0, "\n------------ TRAP IN GETPID CASE ----------------\n");
        KernelGetPid();
        break;

    case YALNIX_BRK:
        TracePrintf(0, "\n------------ TRAP IN BRK CASE ----------------\n");
        addr = user_context->regs[0];
        KernelBrk((void *)addr);
        break;

    case YALNIX_DELAY:
        TracePrintf(0, "\n------------ TRAP IN DELAY CASE ----------------\n");
        clock_ticks = user_context->regs[0];
        KernelDelay(clock_ticks);
        break;

    default:
        break;
    }

    return;
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
    // Extract the type of trigger that triggered the trap.
    int mem_code = user_context->code;
    void *offending_address = user_context->addr;
    TracePrintf(0, "\n------------ memory trap triggered ----------------\n");
    TracePrintf(0, "\nOffending address: %p\n", offending_address);

    // If it is a permission error trigger: return ERROR.
    if (mem_code == YALNIX_ACCERR)
    {
        TracePrintf(0, "\n---------------- CAN'T TOUCH THIS Invalid Permissions ----------------\n");
        KernelExit(mem_code);
    }
    // If it is an address not yet mapped trigger:
    else if (mem_code == YALNIX_MAPERR)
    {
        // If more than two frames below the current pointer, return ERROR.
        if (offending_address < (curr_pcb->memory_context.region_1_sp - 2 * PAGESIZE))
        {
            TracePrintf(0, "\n---------------- CAN'T TOUCH THIS Stack growing more than 2 pages ----------------\n");
            KernelExit(mem_code);
        }

        // Allocate memory for the stack until this new address.
        int high_vpn = (int)curr_pcb->memory_context.region_1_sp >> PAGESHIFT;
        int low_vpn = (int)offending_address >> PAGESHIFT;
        for (int page_id = low_vpn; page_id < high_vpn; page_id++)
        {
            int free_frame = first_free_frame_idx();
            curr_pcb->memory_context.user_page_table->table[page_id].valid = 1;
            curr_pcb->memory_context.user_page_table->table[page_id].prot = PROT_READ | PROT_WRITE;
            curr_pcb->memory_context.user_page_table->table[page_id].pfn = free_frame;
        }
    }
    else
    {
        TracePrintf(0, "\n---------------- CAN'T TOUCH THIS Shouldn't be here ----------------\n");
        KernelExit(mem_code);
    }
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