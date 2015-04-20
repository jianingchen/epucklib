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

void el_initialization();
void el_calibrate_sensors();
void el_sleep(el_time time_ms);
void el_reset();
void el_main_loop();
void el_break_main_loop();

#endif
