/* test_fork_syscall.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Feb 12
 * Version: 1.0
 *
 * Description: Prints Hello World to the Console.
 */

#include <ylib.h>
#include <yuser.h>

int main()
{
    TracePrintf(1, "\n------------Pre Fork-----------------\n");
    TracePrintf(0, "\n------------ Forking process PID: %d ----------------\n", GetPid());
    int pid = Fork();
    if (pid == 0)
    {
        TracePrintf(1, "\n-=-=-=-=-=-=-=In Child-=-=-=-=-=-=-=-=-\n");
    }
    else
    {
        TracePrintf(1, "\n------------ PARENT : Child PID is: %d--------------\n", pid);
    }

    return 0;
}