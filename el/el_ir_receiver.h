
/*!

\defgroup EL_IR_RECEIVER Infrared Receiver

\section Introduction

(TODO)

*/
 
/*!

\file
\ingroup EL_IR_RECEIVER

\author Jianing Chen

*/

#ifndef EL_IR_RECEIVER_H
#define	EL_IR_RECEIVER_H

#include "el_common.h"
#include "el_clock.h"
#include "el_trigger.h"

void el_ir_receiver_reset();
el_uint16 el_ir_receiver_get_counter();
el_uint8 el_ir_receiver_get_check();
el_uint8 el_ir_receiver_get_address();
el_uint8 el_ir_receiver_get_data();


//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"

#define IR_REMOTE_KEY_START     53
#define IR_REMOTE_KEY_STOP      54
#define IR_REMOTE_KEY_PAUSE     41
#define IR_REMOTE_KEY_REW       37
#define IR_REMOTE_KEY_FF        38
#define IR_REMOTE_KEY_REC       55
#define IR_REMOTE_KEY_RESET     12
#define IR_REMOTE_KEY_MUTE      13
#define IR_REMOTE_KEY_PLUS      16
#define IR_REMOTE_KEY_MINUS     17
#define IR_REMOTE_KEY_NUM_0     0
#define IR_REMOTE_KEY_NUM_1     1
#define IR_REMOTE_KEY_NUM_2     2
#define IR_REMOTE_KEY_NUM_3     3
#define IR_REMOTE_KEY_NUM_4     4
#define IR_REMOTE_KEY_NUM_5     5
#define IR_REMOTE_KEY_NUM_6     6
#define IR_REMOTE_KEY_NUM_7     7
#define IR_REMOTE_KEY_NUM_8     8
#define IR_REMOTE_KEY_NUM_9     9
#define IR_REMOTE_KEY_INFO      18
#define IR_REMOTE_KEY_PRESET    14

extern el_uint8 el_irrc_phase;
extern el_mcd el_irrc_timer;

void el_init_ir_receiver();
void el_routine_ir_receiver_14400hz();

#endif	/* EL_INCLUDE_CONTEXT */


#endif	/* EL_IR_RECEIVER_H */
