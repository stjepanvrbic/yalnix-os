/* test_exec.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Feb 12
 * Version: 1.0
 *
 * Description: Tests that exec run this program.
 */

#include <ylib.h>
#include <yuser.h>

int main(int argc, char **argv)
{
    TracePrintf(1, "\n-=-=-=-=-=-=-=-=-= In Test Exec =-=-=-=-=-=-=-=-=-\n");

    TracePrintf(1, "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    TracePrintf(1, "\n        Argc: %d || Argv[0]: %s, Argv[1]: %s\n", argc, argv[0], argv[1]);
    TracePrintf(1, "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    Pause();

    return 0;
}