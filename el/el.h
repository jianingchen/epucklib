/*

embedded system library for e-puck
________________________________________________________________________________

code distribution: 
https://github.com/jianingchen/epucklib

online documentation: 
http://jianingchen.github.io/epucklib/html/
________________________________________________________________________________

This library is released under the terms of the MIT license: 

The MIT License (MIT)

Copyright (c) 2014 Jianing Chen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
________________________________________________________________________________

*/

/*!

\defgroup EL Initialization Functions

\section Introduction

This group contains functions to initialize and host the library. 

*/
 
/*!

\file
\ingroup EL

\brief This is the main header file of this library

\author Jianing Chen

*/

#ifndef EL_H
#define EL_H

#include "el_common.h"
#include "el_clock.h"
#include "el_random.h"
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

/*!
    \brief initialize the library
    
    This function initializes the library. It should only be executed once 
    in the beginning of the program. 
*/
void el_initialization();


/*!
    \brief calibrate the proximity sensors and setup a random seed
    
    Currently, this function calibrate the proximity sensors on the robot as 
    well as generate a random seed based on the noisy sensor readings. 
    If one finds the random seed not diverse enough across a number of robots, 
    ::el_random_set_seed can be used to apply a random seed using any 
    improved method. 
    
    Note: this function takes about 600 ms to finish. 
*/
void el_calibrate_sensors();


/*!
    \brief delay a period of time given in millisecond
    
    This delay function works only within the \c main function but not in any process. 
    Within a process, time delay is achieved by ::el_process_wait. 
*/
void el_sleep(el_time time_ms);


/*!
    \brief the "engine" that drives the software components in this library
    
    This is the primary function that drives all of the software components 
    in this library, including \ref EL_TRIGGER, \ref EL_TIMER and \ref EL_PROCESS. 
    
    The standard way to use this library is that: most of the user's code are 
    written in one or more processes/triggers while the \c main function just 
    enters this function and keeps running it for the life time. 
    See the examples for the practical usage. 
*/
void el_main_loop();


/*!
    \brief signal el_main_loop to stop
    
    This function breaks the loop inside ::el_main_loop and let it returns. 
    At the moment when this function is called, all of the software components 
    are virtually frozen. This means: any process in waiting/cooperating will 
    no longer be resumed and no further trigger/timer actions will happen. 
    
    It is recommended that el_main_loop is only entered once in a program's 
    life time. Thus, after el_main_loop returns, the program should just 
    clear things up and proceed to reset, rather than re-entering el_main_loop 
    at some time. 
*/
void el_break_main_loop();


/*!
    \brief reset the microcontroller
    
    This is equivalent to tap the reset button. 
*/
void el_reset();


#endif
