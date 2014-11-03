
#include "el_context.h"
#include "el_accelerometer.h"

bool el_acc_enabled;
uint8_t el_acc_working_phase;
void (*el_adc_callback_accelerometer)(const unsigned int*result_3v);
uint16_t el_acc_samples[3];

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
    
    el_acc_samples[0] = result_3v[0];
    el_acc_samples[1] = result_3v[1];
    el_acc_samples[2] = result_3v[2];

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

int el_accelerometer_get(int which){
    return el_acc_samples[which];
}

void el_accelerometer_get_all(int*result_3v){
    result_3v[0] = el_acc_samples[0];
    result_3v[1] = el_acc_samples[1];
    result_3v[2] = el_acc_samples[2];
}
