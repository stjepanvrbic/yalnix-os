/* sync_syscalls.c ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30 10:33:29 2021 (-0400)
 * Version: 1.0
 *
 * Description: Implements the synchronization syscalls handlers.
 *
 */

#include "../include/sync_syscalls.h"

/******************************************* LOCKS *******************************************/
int lock_init(int *lock_idp)
{
    // Create the lock
    // If it fails
    //      return ERROR

    // Get an id for the lock
    // If getting id fails
    //      return ERROR

    // Save new id into lock_idp

    // Initialize the wait queue for the lock
    // If initialization fails
    //      return ERROR

    // Save lock somewhere
    // return 0
}

int lock_acquire(/*to be decided*/)
{
    // Find lock
    // If can't find lock
    //      return ERROR

    // If lock already acquired
    //      return 0

    // If the lock is free
    //      get lock
    //      return 0
    // Else lock is not free
    //      add current process to the waiting queue for the lock

    // return 0
}

int lock_release(/*to be decided*/)
{
    // Find lock
    // If can't find lock
    //      return ERROR

    // If the wait queue has processes waiting to acquire the lock
    //      get first process from wait queue
    //      process gets lock
    // Else
    //      set lock free

    // return 0
}

/******************************************* Cvar *******************************************/
int cvar_init(int *cvar_idp)
{
    // Create the cvar
    // If it fails
    //      return ERROR

    // Get an id for the cvar
    // If getting id fails
    //      return ERROR

    // Save new id into cvar_idp

    // Initialize the wait queue for the cvar
    // If initialization fails
    //      return ERROR

    // Save cvar somewhere
    // return 0
}

int cvar_signal(/*to be decided*/)
{
    // Find cvar
    // If can't find cvar
    //      return ERROR

    // If the wait queue has processes waiting
    //      get first process from wait queue
    //      set process to ready queue
    // Else
    //      set cvar free

    // return 0
}

int cvar_broadcast(/*to be decided*/)
{
    // Find cvar
    // If can't find cvar
    //      return ERROR

    // While the wait queue has processes waiting
    //      get first process from wait queue
    //      set process to ready queue

    // return 0
}

int cvar_wait(/*to be decided*/)
{
    // Find cvar
    // If can't find cvar
    //      return ERROR

    // Get lock using the lock_id
    // If can't find lock
    //      return ERROR

    // Release lock

    // Add current process to waiting queue

    // When process is signaled (out of the queue)
    //      reacquire lock
}

int reclaim(/*to be decided*/)
{
    // Find lock, cvar, or pipe
    // If it fails
    //      return ERROR

    // if lock
    //      free all resources allocated
    //      destroy the lock
    //      If it fails
    //          return ERROR
    // else if cvar
    //      free all resources allocated
    //      destroy the cvar
    //      If it fails
    //          return ERROR
    // else if pipe
    //      free all resources allocated
    //      destroy the pipe
    //      If it fails
    //          return ERROR

    // return 0
}