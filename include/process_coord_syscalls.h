/* process_coord_syscalls.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 *
 * Description: Declares the process coordination syscalls handlers.
 *
 */

//---------------------------------- KernelFork ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
int KernelFork(void);

//---------------------------------- KernelExec ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
int KernelExec(char *filename, char **argvec);

//---------------------------------- KernelExit ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void KernelExit(int status);

//---------------------------------- KernelWait ----------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
int KernelWait(int *status_ptr);

//---------------------------------- KernelGetPid --------------------------------
// Description: Returns the pid of the calling process
// Inputs:      nothing
// Outputs:     return the process pid
//--------------------------------------------------------------------------------
int KernelGetPid(void);

//---------------------------------- KernelBrk -----------------------------------
// Description: KernelLand handler for the Region 1 Brk syscall. DIFFERENT FROM SetKernelBrk.
// Inputs:      new address to be set
// Outputs:     return 0 if successfull
//              return ERROR if unsuccessfull
//--------------------------------------------------------------------------------
int KernelBrk(void *addr);

//---------------------------------- KernelDelay ---------------------------------
// Description: Block and wait unitl clock_ticks number of clock interrputs have
//              occured.
// Inputs:      number of clock ticks
// Outputs:     return 0 if successfull
//              return ERROR if unsuccessfull
//--------------------------------------------------------------------------------
int KernelDelay(int clock_ticks);