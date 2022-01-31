 /* KernelLand I/O Syscalls Handlers 
 * 
 * 
 * Author:  Team yeyeye
 * Created: Sat Jan 30 - COSC 58 - 22W
 * Version: 1.0
 * 
 * Description: Declares the data structures used for the I/O syscalls.
 * 
 */

// Difference between read and receive is that read will wait until it can read while receive is triggered.
int TtyRead(int tty_id, void *buf, int len)
{
    /**
     * Read the next line of input from terminal tty_id, copying into buffer buf. Max len of line is provided by len (not null terminated).
     * If there are sufficient unread bytes already waiting
     *      call returns with those.
     * Else 
     *      calling process is blocked until a line of input is available to be returned
     * if length of input is longer, another call will need to be made by the same or another process.
     * if there arent enough bytes provided, only these get returned.
     * 
     */
}

// Use to read data from a terminal when ready (trap will trigger this).
int TtyReceive(int tty_id, void *buf, int len)
{
    /**
     * Copy the new input line from terminal tty_id into buf for a max length of len
     * (buffer needs to be entirely in Region 0)
     * return the actual legnth of the input (including the new line character \n, but excluding the end of file character ctrl+D)
     * len should not exceed TERMINAL_MAX_LINE but this should be taken care off prior to reaching here
     */
}