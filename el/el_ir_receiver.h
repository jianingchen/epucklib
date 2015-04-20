/*

embedded system library for e-puck

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

/*!

\defgroup EL_IR_RECEIVER Infrared Receiver

\section Introduction

The e-puck has a ir receiver module --- TSOP36236. It is supposed to be used with 
remote control based RC5 protocol, which are typically used in TV remote. 

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

void el_enable_ir_receiver();
void el_disable_ir_receiver();

void el_ir_receiver_reset();
el_uint16 el_ir_receiver_get_counter();
el_uint8 el_ir_receiver_get_check();
el_uint8 el_ir_receiver_get_address();
el_uint8 el_ir_receiver_get_data();


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

#include "el_clock.h"

extern el_bool el_irrc_enabled;
extern el_uint8 el_irrc_phase;
extern el_mcd el_irrc_timer;

void el_init_ir_receiver();
void el_routine_ir_receiver_14400hz();
void el_irrc_inhibit(int k);

#endif	/* EL_INCLUDE_LIB_INTERNAL_CONTEXT */


#endif	/* EL_IR_RECEIVER_H */
