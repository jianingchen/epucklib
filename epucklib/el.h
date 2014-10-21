
/*! 

\mainpage embedded system library for e-puck

\section sec_intro Introduction

This library has been developed as a part of the PhD of Jianing Chen in The University of Sheffield.

\section sec_link External links
- http://www.e-puck.org/                 The official site of the e-puck

*/

/*!

\file
\ingroup EL

\brief This is the main header file of this library

\author Jianing Chen

*/

/*!

\defgroup EL Initialization and Misc Functions

\section Introduction

This group contains some misc functions, for example, initialization and time delay.

**/
 
#ifndef EL_H
#define EL_H

#include "el_common.h"
#include "el_clock.h"
#include "el_random.h"
#include "el_list.h"
#include "el_process.h"
#include "el_timer.h"
#include "el_trigger.h"

#include "el_led.h"
#include "el_stepper_motor.h"
#include "el_camera.h"
#include "el_ir_receiver.h"
#include "el_accelerometer.h"
#include "el_ir_proximity.h"
#include "el_uart.h"

EL_API void el_initialization();
EL_API void el_sleep(el_time time_ms);// cannot be used in a process
EL_API int el_get_selector_value();
EL_API void el_main_loop();
EL_API void el_reset();

#endif
