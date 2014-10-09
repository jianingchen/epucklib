
#ifndef EL_STEPPER_MOTOR_H
#define EL_STEPPER_MOTOR_H

#include "el_common.h"

// stepper motor rate is in range [-max,-min] and [min,max]
#define EL_STEPPER_MOTOR_RATE_MAX   1000
#define EL_STEPPER_MOTOR_RATE_MIN   50
#define EL_STEPPER_MOTOR_LEFT               0
#define EL_STEPPER_MOTOR_RIGHT              1
#define EL_STEPPER_MOTOR_GET_LEFT_COUNT     0
#define EL_STEPPER_MOTOR_GET_RIGHT_COUNT    1
#define EL_STEPPER_MOTOR_GET_LEFT_SPEED     2
#define EL_STEPPER_MOTOR_GET_RIGHT_SPEED    3

EL_API void el_enable_stepper_motor(void);
EL_API void el_disable_stepper_motor(void);
EL_API void el_stepper_motor_set(int which,int speed);
EL_API int el_stepper_motor_get(int what);


//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"

#define EL_STPM_NUMBER      2
#define EL_STPM_DIR_STOP    0
#define EL_STPM_DIR_FORWARD 1
#define EL_STPM_DIR_REVERSE 2
#define EL_STPM_INTERRUPT_FREQ  EL_T3_FREQ

typedef struct EL_STEPPER_MOTOR_STRUCT{
    int8_t direction;
    uint8_t phase;
    el_ct period;
    el_ct timer;
    int16_t counter;
} el_stpm;

extern int8_t el_stpm_enabled;
extern el_stpm el_stepper_motors[EL_STPM_NUMBER];

void el_init_stepper_motor();
void el_routine_stepper_motor_14400hz();

#endif


#endif
