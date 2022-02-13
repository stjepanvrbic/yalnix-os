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
#include "pcb.h"

//--------------------------------- LoadProgram ---------------------------------
// Description:
// Inputs:
// Outputs:
//-------------------------------------------------------------------------------
int LoadProgram(char *name, char *args[], pcb_t *proc);
#endif