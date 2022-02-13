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


#include "hardware.h"

//--------------------------------- KCSwitch ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
KernelContext *KCSwitch(KernelContext *kc_in,
                        void *curr_pcb_p,
                        void *next_pcb_p);

//--------------------------------- KCCopy ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
KernelContext *KCCopy(KernelContext *kc_in,
                        void *new_pcb_p,
                        void *not_used);