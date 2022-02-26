/* utils.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 *
 * Description: Implements the process coordination syscalls handlers.
 *
 */

#include <ylib.h>
#include "../include/queue.h"
#include "../include/pcb.h"
#include "../include/utils.h"
#include "../include/kernel_context.h"

void remove_parent(void *elementp)
{
    pcb_t *child_pcb = (pcb_t *)elementp;

    child_pcb->parent_pcb_p = NULL;
}

bool search_pcb(void *elementp, const void *keyp)
{
    pcb_t *element = (pcb_t *)elementp;
    int *key_pid = (int *)keyp;

    if (element->pid == *key_pid)
    {
        return true;
    }
    return false;
}