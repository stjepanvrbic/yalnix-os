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
#include "../include/io_syscalls.h"

int TtyRead(int tty_id, void *buf, int len)
{
    /**
     * Read the next line of input from terminal tty_id, copying into buffer buf.
     * Max len of line is provided by len (not null terminated).
     * If there are sufficient unread bytes already waiting
     *      call returns with those.
     * Else
     *      calling process is blocked until a line of input is available to be returned
     * if length of input is longer, another call will need to be made by the same or another process.
     * if there arent enough bytes provided, only these get returned.
     *
     */
}

int TtyWrite(int tty_id, void *buf, int len)
{
    // Write the contents of buf to the terminal tty_id
    // Block calling process until the buffer has been written down
    // On success return len, in case of error return ERROR
    // If len is greater than TERMINAL_MAX_LINE then do several calls
    // I don't know whether this should be taken care of in the handler
    // or here?
}