/* KernelLand I/O Syscalls Handlers
 *
 *
 * Author:  Team yeyeye
 * Created: Sat Jan 30 - COSC 58 - 22W
 * Version: 1.0
 *
 * Description: Defines the data structures used for the I/O syscalls.
 *
 */

//---------------------------------- TtyRead -------------------------------------
// Description: Difference between read and receive is that read will wait until
//              it can read while receive is triggered.
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
int TtyRead(int tty_id, void *buf, int len);

//--------------------------------- TtyWrite ------------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
int TtyWrite(int tty_id,
             void *buf,
             int len);
