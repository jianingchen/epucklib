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
\ingroup EL_ACC

\author Jianing Chen

*/

#ifndef EL_ACCELEROMETER_H
#define	EL_ACCELEROMETER_H

#include "el_common.h"
#include "el_clock.h"
#include "el_trigger.h"

#define EL_ACCELEROMETER_OUTPUT_MAX     4095U

#define EL_ACCELEROMETER_ONE            0


/*!
    This enum is used in ::el_accelerometer_get to select the output type to get.
*/
typedef enum{
    EL_ACCELERATION_X,          ///< get the acceleration along x-axis
    EL_ACCELERATION_Y,          ///< get the acceleration along y-axis
    EL_ACCELERATION_Z,          ///< get the acceleration along z-axis
    EL_ACCELERATION_ALL_3V,     ///< get the acceleration along all three axes
} el_accelerometer_output;


/*!
    \brief enable the accelerometer

*/
void el_enable_accelerometer();


/*!
    \brief disable the accelerometer

*/
void el_disable_accelerometer();


/*!
    \brief get output value(s) from a accelerometer

    \param i        index of the accelerometer (currently can only be 0)
    \param u        output type
    \param out      pointer to a variable or an array of variables to store 
                    the output value(s). 

    \return         number of values stored into *out.
    
    The refreshing rate of the accelerometer outputs is 120 Hz. 
*/
int el_accelerometer_get(el_index i,el_accelerometer_output u,el_int16*out);


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

extern bool el_acc_enabled;
extern el_int8 el_acc_working_phase;
extern void (*el_adc_callback_accelerometer)(const unsigned int*result_3v);

void el_init_accelerometer();
void el_routine_accelerometer_2400hz(void);

#endif

#endif	/* EL_ACCELEROMETER_H */
