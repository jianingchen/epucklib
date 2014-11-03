
/*! 

\mainpage e-puck embedded system library

\image html el_logo_color.png

\section sec_intro Introduction

This is a library for the e-puck robot platform. 
(TODO)


\n 
\section sec_environment Environment

- MPLAB X with xc16 compiler 
- Support only e-pucks with the camera model PO6030K. 
- Recommend to use with Tiny Bootloader (preferably version 1.10.3)


\n 
\section sec_build Build & Link

\subsection sec_buildlib Build This Library
(TODO)

\subsection sec_project Project Setup
(TODO)


\n 
\section sec_examples Examples

\subsection sec_ex01 Example 1
(TODO)

\subsection sec_ex02 Example 2
(TODO)

\subsection sec_ex03 Example 3
(TODO)


\n 
\section sec_license License & Attributions

The e-puck embedded system library is distributed under the MIT license. 

The source files starting with "e_" are copied from the e-puck standard 
library and attribute to whom ever own the authorship. 

Logo of the e-puck embedded system library (see top left of this page) is 
licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 
International License. 

This library has been developed as a part of the PhD of Jianing Chen in 
Natural Robotics Lab, The University of Sheffield. 

(TODO)


\n 
\section sec_link External Links

- official site of the e-puck robot > http://www.e-puck.org/ 
- homepage of Natural Robotics Lab > http://naturalrobotics.group.shef.ac.uk/ 
- homepage of Tiny Bootloader > http://www.etc.ugal.ro/cchiculita/software/picbootloader.htm 

*/

/*!

\defgroup EL Initialization Functions

\section Introduction

This group contains functions to initialize and host the library. 

**/
 
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
void el_main_loop();
void el_reset();

#endif
