
#include "el_context.h"
#include "el_stepper_motor.h"

int8_t el_stepper_motor_enabled;
el_stpm el_stepper_motors[EL_N_STPM];

static void el_set_stepper_motor_phase(int which,uint8_t phase){
    switch(which){
        
    case 0:// e-puck left motor
        
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
        
    case 1:// e-puck right motor
        
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
    
    el_stepper_motor_enabled = 0;
    
    for(i=0;i<EL_N_STPM;i++){
        p = el_stepper_motors + i;
        p->direction = 0;
        p->phase = 0;
        p->period = 1;
        p->timer = 0;
        p->counter = 0;
        el_set_stepper_motor_phase(i,0xFF);
    }
    
}

void el_enable_stepper_motor(void){
    el_stepper_motor_enabled = true;
}

void el_disable_stepper_motor(void){
    el_stepper_motor_enabled = false;
}

void el_routine_stepper_motor(){
    const el_ct dk = EL_MASTERCLOCK_FREQ/EL_STPM_INTERRUPT_FREQ;// should be 10 exactly
    int i;
    el_stpm *p;
    
    if(el_stepper_motor_enabled){
        
        for(i=0;i<EL_N_STPM;i++){
            
            p = el_stepper_motors + i;
            
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
        
    }else{
        
        for(i=0;i<EL_N_STPM;i++){
            el_set_stepper_motor_phase(i,0xFF);
        }
        
    }
    
}

void el_set_stepper_motor(int which,int speed){
    uint16_t rate;
    uint8_t dir;
    el_ct k;
    int i;
    
    if(which>=EL_N_STPM){
        return;
    }else{
        i = which;
    }
    
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
    
    k = EL_MASTERCLOCK_FREQ/rate;
    
    el_stepper_motors[i].direction = dir;
    el_stepper_motors[i].period = k;
    el_stepper_motors[i].timer = k;
    
    return;
}
