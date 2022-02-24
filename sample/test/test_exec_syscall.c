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
    TracePrintf(1, "\n------------Pre Exec-----------------\n");

    int pid = Fork();
    TracePrintf(0, "\n------------ PID that fork returned: %d ----------------\n", pid);
    if (pid == 0)
    {
        TracePrintf(1, "\n-=-=-=-=-=-=-=In Child-=-=-=-=-=-=-=-=-\n");
        char *args[1];
        args[0] = "./test/test_exec";
        // args[1] = "chicken";
        int rc = Exec("./test/test_exec", args);
        if (rc == ERROR)
        {
            TracePrintf(1, "\n------------Something went wrong while exec-ing --------------\n");
        }
        TracePrintf(1, "\n------------This should never print--------------\n");
    }
    else
    {
        TracePrintf(1, "\n------------ Child that just got forked : My PID is: %d--------------\n", pid);
    }
    while (1)
    {
    }
    return 0;
}