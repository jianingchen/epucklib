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

\file
\ingroup EL_IR_RECEIVER

\author Jianing Chen

*/

#ifndef EL_IR_RECEIVER_H
#define	EL_IR_RECEIVER_H

#include "el_common.h"
#include "el_clock.h"
#include "el_trigger.h"

/*! 
    \brief enable the ir receiver

*/
void el_enable_ir_receiver();


/*! 
    \brief disable the ir receiver

*/
void el_disable_ir_receiver();


/*! 
    \brief reset the ir receiver to prepare for any incoming message

    Once a message is received, the ir receiver must be reset before it can 
    receive any further messages. The general usage is that: always reset the 
    ir receiver before a receiving needed. 
*/
void el_ir_receiver_reset();


/*! 
    \brief get the check bit in the message while 2 means no message
    
    \return 0, 1 and 2
    
    For a common TV remote, the 'check' bit in the message is often used to 
    determine whether the message is issued from a new button click or a 
    re-transmission of a same click (holding a button). 
    For example, if the user hold a button on the remote, the receiver will 
    receive a series of same data with same 'check' bit (0,0,0 ... or 1,1,1 ...). 
    Whereas, it the user is click the button repeatedly, the received messages 
    will contain same data but their 'check' bit will be altered (0,1,0,1,0 ...).
    
    The function will return 2 if there is no message.
    
*/
el_uint8 el_ir_receiver_get_check();


/*! 
    \brief get the address field in the message
    
    \return the address, a 5-bit integer
    
    The message should also contain a address field, which is a 5-bit integer. 
    One can use this to receive the message from a specific remote only. 
    For example, use a TV remote with a unique address number so it won't 
    turn on other appliances in the room where the robots are used. 
    
*/
el_uint8 el_ir_receiver_get_address();


/*! 
    \brief get the data field in the message
    
    \return the data, a 6-bit integer
    
    The payload of a incoming message is the 'data'. It is be a 6-bit integer. 
    When the message is transmitted from a TV remote, the 'data' is usually 
    the code of the key that is pressed. 
    
    In \ref ssec_irrc_usage_1, an example is given to display all contents 
    in a incoming message. 
    
*/
el_uint8 el_ir_receiver_get_data();


/*! 
    \brief get the number of messages received since initialization
    
*/
el_uint16 el_ir_receiver_get_counter();



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
