#pragma once
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
#ifndef _utils_h
#define _utils_h

typedef int bool;
enum
{
    false,
    true
};

typedef struct bit_vector
{
    unsigned char *bit_arr;
} bit_vector_t;

bool search_pcb(void *elementp, const void *keyp);

void remove_parent(void *elementp);

// Stops the current process, adds it to the end of the ready queue,
// and switches to the next ready process. Default process is idle if no
// other process is in the ready queue.
void switch_to_next_ready_process()

    // Set up a way to track free frames (use a bit vector to track this)
    extern bit_vector_t mem_frames; // 0 for free, 1 for used.

#endif
