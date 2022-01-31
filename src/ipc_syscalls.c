 /* ipc_syscalls.c --- 
 * 
 * 
 * Author:  Javier Esteban 
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements the IPC syscall handlers.
 * 
 */

#include "../headers/ipc_syscalls.h"


int pipe_init(int *pipe_idp) {
    // Create pipe
    // If it fails
    //      return ERROR

    // Get an id for the pipe
    // If getting id fails
    //      return ERROR

    // Save new id into pipe_idp

    // Initialize all variables in the pipe struct

    // Save pipe somewhere
    // return 0
}

int pipe_read(/*to be decided*/) {
    // Check the len received is non-negative

    // While the pipe is empty
    //      wait

    // If the pipe has plen <= len
    //      read all the data
    //      If it fails
    //          return ERROR

    // If the pipe has plen > len
    //      read only the up to length len
    //      If it fails
    //          return ERROR

    // Update the necessary variables in the pipe struct

    // return len_bytes_read
}

int pipe_write(/*to be decided*/) {
    // Check the len received is non-negative

    // While the pipe new len + pipe_len > plen
    //      wait for the pipe to be emptier

    // Write all data to the pipe
    // If it fails
    //      return ERROR

    // Update the necessary variables in the pipe struct

    // return len_bytes_written
}