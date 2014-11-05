
#include "el_context.h"
#include "el_stepper_motor.h"

el_bool el_stpm_enabled;
el_bool el_stpm_accel_enabled;
el_int16 el_stpm_accel_cs;
el_int16 el_stpm_accel_linear_term;
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
    el_stpm_accel_cs = 0;
    el_stpm_accel_enabled = false;
    el_stpm_accel_linear_term = 3000/50;
    
    for(i=0;i<EL_STPM_NUMBER;i++){
        p = el_stpm_unit + i;
        p->direction = 0;
        p->phase = 0;
        p->period = 1;
        p->timer = 0;
        p->counter = 0;
        p->ref_speed = 0;
        p->true_speed = 0;
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
        el_stpm_unit[i].timer = 0;
        el_set_stepper_motor_phase(i,0xFF);
    }
}

void el_config_stepper_motor(el_param_stepper_motor e,int v){

    switch(e){

    case EL_SPEED_ACC_ENABLE:
        if(v){
            el_stpm_accel_enabled = true;
        }else{
            el_stpm_accel_enabled = false;
        }
        break;

    case EL_SPEED_ACC_LINEAR_TERM:
        if(v < 100){
            el_stpm_accel_linear_term = 100/50;
        }else
        if(v >= 5000){
            el_stpm_accel_linear_term = 5000/50;
        }else{
            el_stpm_accel_linear_term = v/50;
        }
        break;

    }
    
}

void el_routine_stepper_motor_accel_2400hz(){
    el_stpm *p;
    int i;
    int u,r,e;
    int speed;
    el_uint16 rate;
    el_uint8 dir;
    el_mct period;

    // scale the routine to 50 Hz
    el_stpm_accel_cs++;
    if(el_stpm_accel_cs<48){
        return;
    }else{
        el_stpm_accel_cs = 0;
    }

    // update the stepping rate
    for(i=0;i<EL_STPM_NUMBER;i++){
        p = el_stpm_unit + i;
        
        r = p->ref_speed;
        u = p->true_speed;

        e = r - u;

        if(e==0){
            continue;
        }else
        if(e > 0){
            speed = u + el_stpm_accel_linear_term;
            if(speed > r){
                speed = r;
            }
        }else{
            speed = u - el_stpm_accel_linear_term;
            if(speed < r){
                speed = r;
            }
        }
        
        if(speed > EL_STEPPER_MOTOR_RATE_MAX){
            speed = EL_STEPPER_MOTOR_RATE_MAX;
            rate = EL_STEPPER_MOTOR_RATE_MAX;
            dir = EL_STPM_DIR_FORWARD;
        }else
        if(speed < -EL_STEPPER_MOTOR_RATE_MAX){
            speed = -EL_STEPPER_MOTOR_RATE_MAX;
            rate = EL_STEPPER_MOTOR_RATE_MAX;
            dir = EL_STPM_DIR_REVERSE;
        }
        
        p->true_speed = speed;

        if(speed >= EL_STEPPER_MOTOR_RATE_MIN){
            rate = speed;
            dir = EL_STPM_DIR_FORWARD;
        }else
        if(speed <= -EL_STEPPER_MOTOR_RATE_MIN){
            rate = -speed;
            dir = EL_STPM_DIR_REVERSE;
        }else{
            rate = 50;
            dir = EL_STPM_DIR_STOP;
        }

        period = EL_MASTERCLOCK_FREQ/rate;

        p->direction = dir;
        p->period = period;

    }

}

void el_routine_stepper_motor_14400hz(){
    const el_mcd dk = EL_MASTERCLOCK_FREQ/14400;// should be 10 exactly
    el_stpm *p;
    int i;
    
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
    el_uint16 rate;
    el_uint8 dir;
    el_mct period;
    
    if(speed > EL_STEPPER_MOTOR_RATE_MAX){
        speed = EL_STEPPER_MOTOR_RATE_MAX;
        rate = EL_STEPPER_MOTOR_RATE_MAX;
        dir = EL_STPM_DIR_FORWARD;
    }else
    if(speed < -EL_STEPPER_MOTOR_RATE_MAX){
        speed = -EL_STEPPER_MOTOR_RATE_MAX;
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
        speed = 0;
        rate = 50;
        dir = EL_STPM_DIR_STOP;
    }
    
    if(el_stpm_accel_enabled){
        el_stpm_unit[i].ref_speed = speed;
    }else{
        period = EL_MASTERCLOCK_FREQ/rate;
        el_stpm_unit[i].direction = dir;
        el_stpm_unit[i].period = period;
        el_stpm_unit[i].timer = period;
        el_stpm_unit[i].true_speed = speed;
        el_stpm_unit[i].ref_speed = speed;
    }
    
}

void el_stepper_motor_set_counter(el_index which,int steps){
    int i = which%EL_STPM_NUMBER;
    el_stpm_unit[i].counter = steps;
}

int el_stepper_motor_get_counter(el_index which){
    int i = which%EL_STPM_NUMBER;
    return el_stpm_unit[i].counter;
}

void el_set_wheel_speed(int left,int right){
    el_stepper_motor_set_speed(EL_STEPPER_MOTOR_LEFT,left);
    el_stepper_motor_set_speed(EL_STEPPER_MOTOR_RIGHT,right);
}
