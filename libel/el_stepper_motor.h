
/*!

\defgroup EL_STEPPER_MOTOR Stepper Motor

\section Introduction

The e-puck driven by two stepper-motors. 

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

void el_stepper_motor_set_speed(el_index which,int speed);
void el_stepper_motor_set_counter(el_index which,int steps);
int el_stepper_motor_get_counter(el_index which);

void el_enable_stepper_motor(void);
void el_disable_stepper_motor(void);

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
