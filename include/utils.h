/* utils.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Jan 31
 * Version: 1.0
 *
 * Description: Declares the supporting data structures used to construct the OS.
 *
 *
 */

#include "../include/hardware.h"
#include "../include/traps.h"
#include "../include/yalnix.h"
#include "../include/ykernel.h"
#include "../include/pcb.h"
#include "../include/kernel_context.h"

typedef struct queue
{
    // Queue structure
    // Reusing queue from ENGS 50
    // The queue only uses malloc and free
} queue_t;

typedef struct bit_vector
{
    unsigned char *bit_arr;
} bit_vector_t;
