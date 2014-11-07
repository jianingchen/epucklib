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

#include "el_context.h"
#include "el_accelerometer.h"

bool el_acc_enabled;
el_int8 el_acc_working_phase;
el_int16 el_acc_samples[3];
void (*el_adc_callback_accelerometer)(const unsigned int*result_3v);

void el_init_accelerometer(){

    el_acc_enabled = false;
    el_acc_working_phase = 0;
    el_adc_callback_accelerometer = NULL;

    el_acc_samples[0] = 0;
    el_acc_samples[1] = 0;
    el_acc_samples[2] = 0;

}

void el_enable_accelerometer(){
    el_acc_working_phase = 0;
    el_acc_enabled = true;
}

void el_disable_accelerometer(){
    el_acc_enabled = false;
    el_acc_working_phase = 0;
    el_acc_samples[0] = 0;
    el_acc_samples[1] = 0;
    el_acc_samples[2] = 0;
}

void el_routine_accelerometer_adc(const unsigned int*result_3v){

    el_adc_callback_accelerometer = NULL;
    
    el_acc_samples[0] = (el_int16)result_3v[0];
    el_acc_samples[1] = (el_int16)result_3v[1];
    el_acc_samples[2] = (el_int16)result_3v[2];
    
}

void el_routine_accelerometer_2400hz(void){

    if(el_acc_working_phase==0){
        el_adc_callback_accelerometer = el_routine_accelerometer_adc;
    }else
    if(el_acc_working_phase==1){
        /// signal a trigger event
        el_trg_event_flag_ex_acc = 1;
    }

    el_acc_working_phase++;
    if(el_acc_working_phase>=20){
        el_acc_working_phase = 0;
    }

}

int el_accelerometer_get(el_index i,el_accelerometer_output u,el_int16*out){

    switch(u){

    case EL_ACCELERATION_X:
        *out = el_acc_samples[0];
        return 1;

    case EL_ACCELERATION_Y:
        *out = el_acc_samples[1];
        return 1;

    case EL_ACCELERATION_Z:
        *out = el_acc_samples[2];
        return 1;

    case EL_ACCELERATION_ALL_3V:
        out[0] = el_acc_samples[0];
        out[1] = el_acc_samples[1];
        out[2] = el_acc_samples[2];
        return 3;
    }

    return 0;
}
