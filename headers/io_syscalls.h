/* io_syscalls.h --- 
 * 
 * 
 * Author:  team yeyeye
 * Created: Jan 31
 * Version: 1.0
 * 
 * Description: Declares the data structures used for the IO syscalls.
 *             
 * 
 */

//--------------------------------- TtyWrite ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int TtyWrite(int tty_id,
             void *buf,
             int len);

//--------------------------------- TtyTransmit ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
void TtyTransmit(int tty_id,
                 void *buf,
                 int len);