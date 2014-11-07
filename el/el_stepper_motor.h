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

\defgroup EL_STEPPER_MOTOR Stepper Motor

\section Introduction

The e-puck is driven by two stepper-motors. 
The maximum speed of these motors are 1000 steps per second. 

*/
 
/*!

\file
\ingroup EL_STEPPER_MOTOR

\author Jianing Chen

*/

#ifndef EL_STEPPER_MOTOR_H
#define EL_STEPPER_MOTOR_H

#include "el_common.h"

// stepper motor rate is in range [-max,-min] and [min,max]
#define EL_STEPPER_MOTOR_RATE_MAX   1000
#define EL_STEPPER_MOTOR_RATE_MIN   50

#define EL_STEPPER_MOTOR_LEFT               0
#define EL_STEPPER_MOTOR_RIGHT              1


/*!
    This data structure is used in ::el_config_stepper_motor.
    A static instance of this struct exists internally, it can be pointed to
    through ::el_config_stepper_motor_list. 
*/
typedef struct {
    el_bool UseAcceleration;
    el_uint16 AccelerationRate;
} el_stepper_motor_param;

/*
--------------------------------------------------------------------------------
*/

/*!
    \brief get the pointer to a static instance of \ref el_stepper_motor_param

    \return the pointer
*/
el_stepper_motor_param*el_config_stepper_motor_list();


/*!
    \brief apply the parameters to the stepper motors

    \param p    pointer to the data structure containing all parameters

    Currently, the parameters are mainly related to the artificial acceleration
    of the stepper motors, which are typically used to make the motion of
    the robot more smooth.

    For example, the following code enables a linear artificial acceleration
    with a rate of 2000:
    \code
    el_stepper_motor_param *StepperMotorSetting;
    ...
    StepperMotorSetting = el_config_stepper_motor_list();
    StepperMotorSetting->UseAcceleration = true;
    StepperMotorSetting->AccelerationRate = 2000;
    el_config_stepper_motor( StepperMotorSetting );
    ...
    \endcode
    A linear acceleration rate of 2000 means the stepping rate of the motors 
    can reach 1000 from 0 in 0.5 sec. By default, the artificial acceleration
    is enabled and the acceleration rate is 3000. 
*/
void el_config_stepper_motor(const el_stepper_motor_param*p);


/*! 
    \brief enable the stepper motor module
*/
void el_enable_stepper_motor(void);


/*! 
    \brief disable the stepper motor module
*/
void el_disable_stepper_motor(void);


/*
--------------------------------------------------------------------------------
*/


/*! 
    \brief set the stepping rate and direction of a motor

    \param i    index of the motor
    \param u    stepping rate of the motor
    
    The valid range of the stepping speed is in [-1000,-50] and [50,1000]. 
    This function does handle the range issue internally. 
*/
void el_stepper_motor_set_speed(el_index i,int u);


/*! 
    \brief get the step counter of a motor

    \param i    index of the motor
    
    \return u   number of steps
    
*/
int el_stepper_motor_get_counter(el_index i);


/*! 
    \brief set the step counter of a motor

    \param i    index of the motor
    \param u    number of steps
    
*/
void el_stepper_motor_set_counter(el_index i,int n);


/*! 
    \brief change the rotation speed for both of the motors on the e-puck

    \param left    left motor speed
    \param right   right motor speed
    
*/
void el_set_wheel_speed(int left,int right);


//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"

#define EL_STPM_NUMBER              2
#define EL_STPM_DIR_STOP            0
#define EL_STPM_DIR_FORWARD         1
#define EL_STPM_DIR_REVERSE         2
#define EL_STPM_INTERRUPT_FREQ      EL_T3_FREQ
#define EL_STPM_SPEED_CONTROL_FREQ  50

typedef struct{
    el_int8 direction;
    el_int8 phase;
    el_mcd period;
    el_mcd timer;
    el_int16 counter;
    el_int16 true_speed;
    el_int16 ref_speed;
} el_stpm;

extern el_bool el_stpm_enabled;
extern el_bool el_stpm_accel_enabled;

void el_init_stepper_motor();
void el_routine_stepper_motor_14400hz();
void el_routine_stepper_motor_accel_2400hz();

#endif


#endif
