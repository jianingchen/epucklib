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

\defgroup EL_LED LED and Selector

\section Introduction

The LEDs around the body of the e-puck and the selector switch on the top
board of the e-puck. 

*/
 
/*!

\file
\ingroup EL_LED

\author Jianing Chen

*/

#ifndef EL_LED_H
#define EL_LED_H

#include "el_common.h"

#define EL_LED_RING_ALL -1
#define EL_LED_RING_0   0
#define EL_LED_RING_1   1
#define EL_LED_RING_2   2
#define EL_LED_RING_3   3
#define EL_LED_RING_4   4
#define EL_LED_RING_5   5
#define EL_LED_RING_6   6
#define EL_LED_RING_7   7
#define EL_LED_BODY     9
#define EL_LED_FRONT    10

/// used in ::el_led_set. 
typedef enum{
    EL_OFF = 0,     ///< turn off the LED
    EL_ON = 1,      ///< turn on the LED
    EL_TOGGLE = -1, ///< toggle the on/off status of the LED
} el_led_action;


/*! 
    \brief set the status of a LED

    \param i    index of the LED
    \param s    status
    
    See \ref PAGE_COMP_INDEX for the index of each of the LEDs.
*/
void el_led_set(el_index w,el_led_action s);


/*! 
    \brief get the selector postition

    \return the selector position ranged in [0,15]
    
*/
int el_get_selector_value();


#endif
