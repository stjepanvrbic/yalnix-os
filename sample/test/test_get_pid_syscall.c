/* test_get_pid_syscall.c ---
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
    int pid = GetPid();
    while (1)
    {

        TracePrintf(1, "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
        TracePrintf(1, "\n            My PID is: %d\n", pid);
        Pause();
    }
    return 0;
}