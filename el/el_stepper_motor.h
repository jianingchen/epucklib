
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
    \brief enable the stepper motor module

*/
void el_enable_stepper_motor(void);


/*! 
    \brief disable the stepper motor module

*/
void el_disable_stepper_motor(void);


/*! 
    \brief set the stepping rate of motor i

    \param i    index of the motor
    \param u    stepping rate of the motor
    
    The valid range of the stepping speed is in [-1000,-50] and [50,1000]. 
    This function does handle the range issue internally. 
*/
void el_stepper_motor_set_speed(el_index i,int u);


/*! 
    \brief get the step counter of motor i

    \param i    index of the motor
    
    \return u    number of steps
    
*/
int el_stepper_motor_get_counter(el_index i);

/*! 
    \brief set the step counter of motor i

    \param i    index of the motor
    \param u    number of steps
    
*/
void el_stepper_motor_set_counter(el_index i,int n);


void el_set_wheel_speed(int left,int right);


//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"

#define EL_STPM_NUMBER      2
#define EL_STPM_DIR_STOP    0
#define EL_STPM_DIR_FORWARD 1
#define EL_STPM_DIR_REVERSE 2
#define EL_STPM_INTERRUPT_FREQ  EL_T3_FREQ

typedef struct{
    int8_t direction;
    uint8_t phase;
    el_mcd period;
    el_mcd timer;
    int16_t counter;
} el_stpm;

extern int8_t el_stpm_enabled;

void el_init_stepper_motor();
void el_routine_stepper_motor_14400hz();

#endif


#endif
