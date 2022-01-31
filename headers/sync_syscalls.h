 /* sync_syscalls.h --- 
 * 
 * 
 * Author:  team yeyeye 
 * Created: Sat Jan 30
 * Version: 1.0
 * 
 * Description: Declares the data structures used for the synchronization syscalls.
 *              Declares the handlers used for the synchronization syscalls.
 * 
 */

// INLCUDE NECESSARY FILES FROM YALNIX FRAMEWORK


typedef struct Lock {
    int id;
    queue_t wait_q;     // processes waiting to acquire the lock
    pcb_t process;
} lock_t;

typedef struct Cvar {
    int id;
    queue_t wait_q;     // processes waiting to get signaled
    pcb_t process;
} cvar_t;


//---------------------------------- lock_init -----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int lock_init(int *lock_idp);

//-------------------------------- lock_acquire ----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int lock_acquire(/*to be decided*/);

//-------------------------------- lock_release ----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int lock_release(/*to be decided*/);


//--------------------------------- cvar_init ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int cvar_init(int *cvar_idp);

//-------------------------------- cvar_signal -----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int cvar_signal(/*to be decided*/);

//------------------------------- cvar_broadcast ---------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int cvar_broadcast(/*to be decided*/);

//---------------------------------- cvar_wait -----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int cvar_wait(/*to be decided*/);


//---------------------------------- reclaim -------------------------------------
// Description:  
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int reclaim(/*to be decided*/);