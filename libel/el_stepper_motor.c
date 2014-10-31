
#include "el_context.h"
#include "el_stepper_motor.h"

int8_t el_stpm_enabled;
el_stpm el_stpm_unit[EL_STPM_NUMBER];

static void el_set_stepper_motor_phase(el_index which,uint8_t phase){
    switch(which){
        
    case EL_STEPPER_MOTOR_LEFT:
        
        switch(phase){
        case 0:
            MOTOR1_PHA = 1;
            MOTOR1_PHB = 0;
            MOTOR1_PHC = 0;
            MOTOR1_PHD = 1;
            break;
        case 1:
            MOTOR1_PHA = 1;
            MOTOR1_PHB = 0;
            MOTOR1_PHC = 1;
            MOTOR1_PHD = 0;
            break;
        case 2:
            MOTOR1_PHA = 0;
            MOTOR1_PHB = 1;
            MOTOR1_PHC = 1;
            MOTOR1_PHD = 0;
            break;
        case 3:
            MOTOR1_PHA = 0;
            MOTOR1_PHB = 1;
            MOTOR1_PHC = 0;
            MOTOR1_PHD = 1;
            break;
        default:
            MOTOR1_PHA = 0;
            MOTOR1_PHB = 0;
            MOTOR1_PHC = 0;
            MOTOR1_PHD = 0;
            break;
        }
        break;
        
    case EL_STEPPER_MOTOR_RIGHT:
        
        switch(phase){
        case 0:    
            MOTOR2_PHA = 0;
            MOTOR2_PHB = 1;
            MOTOR2_PHC = 0;
            MOTOR2_PHD = 1;
            break;
        case 1:
            MOTOR2_PHA = 0;
            MOTOR2_PHB = 1;
            MOTOR2_PHC = 1;
            MOTOR2_PHD = 0;
            break;
        case 2:    
            MOTOR2_PHA = 1;
            MOTOR2_PHB = 0;
            MOTOR2_PHC = 1;
            MOTOR2_PHD = 0;
            break;
        case 3:    
            MOTOR2_PHA = 1;
            MOTOR2_PHB = 0;
            MOTOR2_PHC = 0;
            MOTOR2_PHD = 1;
            break;
        default:    
            MOTOR2_PHA = 0;
            MOTOR2_PHB = 0;
            MOTOR2_PHC = 0;
            MOTOR2_PHD = 0;
            break;
        }
        break;
    }
    
}

void el_init_stepper_motor(){
    int i;
    el_stpm *p;
    
    el_stpm_enabled = 0;
    
    for(i=0;i<EL_STPM_NUMBER;i++){
        p = el_stpm_unit + i;
        p->direction = 0;
        p->phase = 0;
        p->period = 1;
        p->timer = 0;
        p->counter = 0;
        el_set_stepper_motor_phase(i,0xFF);
    }
    
}

void el_enable_stepper_motor(void){
    el_stpm_enabled = true;
}

void el_disable_stepper_motor(void){
    int i;
    el_stpm_enabled = false;
    for(i=0;i<EL_STPM_NUMBER;i++){
        el_set_stepper_motor_phase(i,0xFF);
    }
}

void el_routine_stepper_motor_14400hz(){
    const el_mcd dk = EL_MASTERCLOCK_FREQ/14400;// should be 10 exactly
    int i;
    el_stpm *p;
    
    for(i=0;i<EL_STPM_NUMBER;i++){

        p = el_stpm_unit + i;

        if(p->timer > 0){
            p->timer -= dk;
        }

        if(p->timer <= 0){

            p->timer += p->period;

            if(p->direction==EL_STPM_DIR_FORWARD){
                p->counter++;
                p->phase++;
                p->phase &= 0x03;
                el_set_stepper_motor_phase(i,p->phase);
            }else
            if(p->direction==EL_STPM_DIR_REVERSE){
                p->counter--;
                p->phase--;
                p->phase &= 0x03;
                el_set_stepper_motor_phase(i,p->phase);
            }else{
                el_set_stepper_motor_phase(i,0xFF);
            }

        }

    }
}

void el_stepper_motor_set_speed(el_index which,int speed){
    int i = which%EL_STPM_NUMBER;
    uint16_t rate;
    uint8_t dir;
    el_mcd period;
    
    if(speed > EL_STEPPER_MOTOR_RATE_MAX){
        rate = EL_STEPPER_MOTOR_RATE_MAX;
        dir = EL_STPM_DIR_FORWARD;
    }else
    if(speed < -EL_STEPPER_MOTOR_RATE_MAX){
        rate = EL_STEPPER_MOTOR_RATE_MAX;
        dir = EL_STPM_DIR_REVERSE;
    }else
    if(speed >= EL_STEPPER_MOTOR_RATE_MIN){
        rate = speed;
        dir = EL_STPM_DIR_FORWARD;
    }else
    if(speed <= -EL_STEPPER_MOTOR_RATE_MIN){
        rate = -speed;
        dir = EL_STPM_DIR_REVERSE;
    }else{
        rate = 1000;
        dir = EL_STPM_DIR_STOP;
    }
    
    period = EL_MASTERCLOCK_FREQ/rate;
    
    el_stpm_unit[i].direction = dir;
    el_stpm_unit[i].period = period;
    el_stpm_unit[i].timer = period;
    
    return;
}

void el_stepper_motor_set_counter(el_index which,int steps){
    int i = which%EL_STPM_NUMBER;
    el_stpm_unit[i].counter = steps;
}

int el_stepper_motor_get_counter(el_index which){
    int i = which%EL_STPM_NUMBER;
    return el_stpm_unit[i].counter;
}
