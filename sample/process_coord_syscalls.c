// KernelLand Process Coordination Syscalls Handlers
// Selim Hassairi
// COSC 58 - 22W

int KernelFork(void)
{
    /**
     * Create a new process with its new virtual memory space.
     *     KernelStart needs to create an initPCB with:
     *         region 1 page table, all invalid
     *         new frames from its kernel and stack frames.
     *         a UserContext, which is obtained from the uctxt arg to KernelStart
     *         a new pid, obtainable through helper_new_pid()
     * Copy the content of the parent process to the forked child.
     *     KCCopy() to copy kernelcontext into initPCB
     *     copy the contents of the current kernel stack into the new kernel stack frames in initPCB
     * How do we deal with returning two different things in two different processes?
     *     Maybe change somehow change something in the process of the child that will make it return 0 and push that process to the queue of processes?
     *     The parent can just easily return the pid of the child process.
     * If copying or creating the new process fails, return value ERROR.
    */
}

int KernelExec(char filename, char *argvec)
{
    /**
     * Wipe out the current process' Region 1
     * Start the execution of the program with the given filename while passing it its arguements argvec. 
    */
}

void KernelExit(int status)
{
    /**
     * Save status value
     * Free all the resources used by that process
     * When parent exits, their children keep running.
     * When orphans exits, no need to save or report status.
    */
}

int KernelWait(int status_ptr)
{
    /**
     * Collects process ID and exit status returned by a child of the program calling.
     * If caller has an exited child whose info hasnt been collected yet by Wait, return it directly.
     * If there are no child processes to wait for, return with ERROR.
     * If there are children processes not terminated, block and wait for exit info to return.
     * On success, the pid of the child is returned and status ptr is written to.
    */
}

int KernelGetPid(void)
{
    // Returns the pid of the calling process
}

int KernelBrk(void *addr)
{
    /**
     * Set the brk to addr (rounded up to the next multiple of PAGESIZE bytes).
     * Return 0 if succes.
     * If the addr is invalid or if not enough memory is available, the value ERROR is returned.
     */
}

int KernelDelay(int clock_ticks)
{
    /**
     * Block and wait unitl clock_ticks number of clock interrputs have occurec.
     * Upon completion return 0
     * If clock_ticks < 0 return ERROR.
     */
}