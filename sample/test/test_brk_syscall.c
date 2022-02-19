/* test_brk_syscall.c ---
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
    int size = 100;
    while (1)
    {
        TracePrintf(1, "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
        TracePrintf(1, "\n----------about to malloc----------\n");
        int *test_malloc = malloc(sizeof(int) * size);
        Delay(10000000);
        if (test_malloc == NULL)
        {
            TracePrintf(1, "\n----------malloc failed----------\n");
        }
        TracePrintf(1, "\n----------malloc succesful----------\n");
    }
}