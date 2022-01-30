// KernelLand Syscalls Handlers

int Fork(void)
{
    /*
    Create a new process with its new virtual memory space.
        KernelStart needs to create an initPCB with:
            region 1 page table, all invalid
            new frames from its kernel and stack frames.
            a UserContext, which is obtained from the uctxt arg to KernelStart
            a new pid, obtainable through helper_new_pid()
    Copy the content of the parent process to the forked child.
        KCCopy() to copy kernelcontext into initPCB
        copy the contents of the current kernel stack into the new kernel stack frames in initPCB
    How do we deal with returning two different things in two different processes?
        Maybe change somehow change something in the process of the child that will make it return 0 and push that process to the queue of processes?
        The parent can just easily return the pid of the child process.
    If copying or creating the new process fails, return value ERROR.
    */
}

int Exec(char filename, char *argvec)
{
    /*
    Wipe out the current process' Region 1
    Start the execution of the program with the given filename while passing it its arguements argvec. 
    */
}

void Exit(int status)
{
    /*
    Save status value
    Free all the resources used by that process
    When parent exits, their children keep running.
    When orphans exits, no need to save or report status.
    */
}

int Wait(int status ptr)
{
    /*
    Collects process ID and exit status returned by a child of the program calling.
    If caller has an exited child whose info hasnt been collected yet by Wait, return it directly.
    If there are no child processes to wait for, return with ERROR.
    If there are children processes not terminated, block and wait for exit info to return.
    On success, the pid of the child is returned and status ptr is written to.
    */
}

int GetPid(void)
{
    // Returns the pid of the calling process
}

int Brk(void *addr)
{

}

int Delay(int clock_ticks) {
    /*
    Block and wait unitl clock_ticks number of clock interrputs have occurec.
    Upon completion return 0
    IF clock_ticks < 0 return ERROR.
    */
}