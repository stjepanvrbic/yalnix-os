/* traps.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Sat Jan 30
 * Version: 1.0
 *
 * Description: Declares the handlers used for the traps.
 */

#include "hardware.h"

//---------------------------- trap_kernel_handler -------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_kernel_handler(UserContext *user_context);

//---------------------------- trap_clock_handler --------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_clock_handler(UserContext *user_context);

//---------------------------- trap_illegal_handler ------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_illegal_handler(UserContext *user_context);

//---------------------------- trap_memory_handler -------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_memory_handler(UserContext *user_context);

//----------------------------- trap_math_handler --------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_math_handler(UserContext *user_context);

//-------------------------- trap_tty_receive_handler ----------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_tty_receive_handler(UserContext *user_context);

//------------------------- trap_tty_transmit_handler ----------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_tty_transmit_handler(UserContext *user_context);

//----------------------------- trap_disk_handler --------------------------------
// Description:
// Inputs:
// Outputs:
//--------------------------------------------------------------------------------
void trap_disk_handler(UserContext *user_context);