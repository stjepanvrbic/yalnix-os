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
    TracePrintf(1, "\n------------ ABOUT TO DIVIDE BY 0-----------------\n");
    float x = 2 / 0;

    return 0;
}