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
    TracePrintf(1, "\n------------Pre Exec-----------------\n");
    char *args[2] = {
        "echo",
        "donsos chicken"};
    int rc = Exec("echo", args);
    if (rc == ERROR)
    {
        TracePrintf(1, "\n------------Something went wrong while exec-ing --------------\n");
    }
    TracePrintf(1, "\n------------This should never print--------------\n");
    return 0;
}