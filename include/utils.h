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

// Set up a way to track free frames (use a bit vector to track this)
extern bit_vector_t mem_frames; // 0 for free, 1 for used.

#endif
