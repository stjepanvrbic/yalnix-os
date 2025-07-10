#pragma once
/* kernel_context.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Jan 31
 * Version: 1.0
 *
 * Description: Declares the data structures used for the kernel context.
 *
 *
 */
#ifndef _kernel_context_h
#define _kernel_context_h
#include "hardware.h"

//--------------------------------- KCSwitch ------------------------------------
// Description:
// Inputs:
// Outputs:
//-------------------------------------------------------------------------------
KernelContext *KCSwitch(KernelContext *kc_in,
                        void *curr_pcb_p,
                        void *next_pcb_p);

//--------------------------------- KCCopy --------------------------------------
// Description:
// Inputs:
// Outputs:
//-------------------------------------------------------------------------------
KernelContext *KCCopy(KernelContext *kc_in,
                      void *new_pcb_p,
                      void *not_used);

// Stops the current process, adds it to the end of the ready queue,
// and switches to the next ready process. Default process is idle if no
// other process is in the ready queue.
//------------------------ switch_to_next_ready_process -------------------------
// Description:
// Inputs:
// Outputs:
//-------------------------------------------------------------------------------
void switch_to_next_ready_process();

#endif