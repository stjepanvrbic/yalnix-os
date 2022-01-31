// KernelLand Process Coordination Syscalls Handlers
// Selim Hassairi
// COSC 58 - 22W

int KernelFork(void);

int KernelExec(char filename, char *argvec);

void KernelExit(int status);

int KernelWait(int status_ptr);

int KernelGetPid(void);

int KernelBrk(void *addr);

int KernelDelay(int clock_ticks);