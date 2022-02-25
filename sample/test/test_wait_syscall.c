/* test_exec_syscall.c ---
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
    TracePrintf(1, "\n------------PARENT: Pre Fork-----------------\n");
    int pid = Fork();
    if (pid == 0)
    {
        TracePrintf(1, "\n-=-=-=-=-=-=-=In Child-=-=-=-=-=-=-=-=-\n");
        for (int i = 0; i < 1000; i++)
        {
            TracePrintf(1, "\n------------ CHILD: doing stuff: %d--------------\n", i);
        }
        return 0;
    }

    TracePrintf(1, "\n------------ PARENT: Waiting for kid with pid : %d--------------\n", pid);
    int status;
    int err = Wait(&status);
    if (err == ERROR)
    {
        TracePrintf(1, "\n------------ PARENT: WE GOT A PROBLEM : --------------\n");
    }
    // TracePrintf(1, "\n------------ PARENT: Stopped waiting, status is : %d--------------\n", status);
    TracePrintf(1, "\n------------ PARENT: Stopped waiting, status is : %d--------------\n", status);

    return 0;
}