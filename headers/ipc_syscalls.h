 /* ipc_syscalls.h --- 
 * 
 * 
 * Author:  Javier Esteban 
 * Created: Sat Jan 30
 * Version: 1.0
 * 
 * Description: Declares the data structures used for the IPC syscalls.
 *              Declares the handlers used for the IPC syscalls.
 * 
 */

// INLCUDE NECESSARY FILES FROM YALNIX FRAMEWORK


typedef struct Pipe {
    int id;
    char *buffer;
    int max_len;
    int current_len;
    queue_t read_q;         // for processes waiting to read from the pipe
    queue_t write_q;        // for processes waiting to write to the pipe
} pipe_t;


//--------------------------------- pipe_init ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int pipe_init(int *lock_idp);

//--------------------------------- pipe_read ------------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int pipe_read(/*to be decided*/);

//--------------------------------- pipe_write -----------------------------------
// Description:   
// Inputs:        
// Outputs:       
//--------------------------------------------------------------------------------
int pipe_write(/*to be decided*/);