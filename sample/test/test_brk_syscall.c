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
    int size = 10000;

    TracePrintf(1, "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    TracePrintf(1, "\n----------about to malloc----------\n");
    int *test_malloc = malloc(sizeof(int) * size);
    // Using new allocated memory
    int i;
    for (i = 0; i < size; i++)
    {
        test_malloc[i] = i;
        TracePrintf(1, "\n----------- Test malloc[%d]: %d ---------\n", i, i);
    }
    if (test_malloc == NULL)
    {
        TracePrintf(1, "\n----------malloc failed----------\n");
    }
    TracePrintf(1, "\n----------malloc succesful----------\n");
    while (1)
    {
    }
}