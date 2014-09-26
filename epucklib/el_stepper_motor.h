
#ifndef EL_STEPPER_MOTOR_H
#define EL_STEPPER_MOTOR_H

#ifdef EL_INCLUDE_CONTEXT

#define EL_N_STPM    2
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

extern int8_t el_stepper_motor_enabled;
extern el_stpm el_stepper_motors[EL_N_STPM];

void el_init_stepper_motor();
void el_routine_stepper_motor();

#endif

// stepper motor rate is in range [-max,-min] and [min,max]
#define EL_STEPPER_MOTOR_RATE_MAX   1000
#define EL_STEPPER_MOTOR_RATE_MIN   50

EL_API void el_enable_stepper_motor(void);
EL_API void el_disable_stepper_motor(void);
EL_API void el_set_stepper_motor(int which,int speed);

#endif
