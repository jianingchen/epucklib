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
#include "el_interrupt.h"
#include "el.h"

volatile int el_mlb;

//------------------------------------------------------------------------------

void el_initialization(){

    e_init_port();
    e_i2cp_init();

    el_init_masterclock();
    el_init_timers();
    el_init_process();
    el_init_triggers();

    el_init_uart();
    el_init_stepper_motor();
    el_init_ir_receiver();
    el_init_accelerometer();
    el_init_ir_proximity();
    el_init_camera();

    el_init_interrupt_ADC();
    el_init_interrupt_INT0();
    el_init_interrupt_T1();
    el_init_interrupt_T2();
    el_init_interrupt_T3();
    el_init_interrupt_T4();
    el_init_interrupt_T5();
    el_init_interrupt_UART();

}

NEVER_RETURN el_main_loop(){
    
    el_process_mck = el_get_masterclock();
    el_timer_mck = el_get_masterclock();

    el_mlb = 1;
    
    while(el_mlb){
        el_routine_timers();
        el_routine_triggers();
        el_routine_process();
    }
    
}

void el_break_main_loop(){
    el_mlb = 0;
}

void el_reset(){
    RESET();
}

void el_sleep(el_time time_ms){
    el_mct k;
    if(el_is_in_process||el_is_in_timer_callback||el_is_in_trigger_condition){
        return;
    }
    k = el_get_masterclock() + EL_TIME_TO_MCT(time_ms);
    while(el_get_masterclock() < k){
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
    }
}

void el_calibrate_sensors(){
    int i,n;
    int x,w;


    el_led_set(EL_LED_RING_1,EL_ON);
    el_led_set(EL_LED_RING_3,EL_ON);
    el_led_set(EL_LED_RING_5,EL_ON);
    el_led_set(EL_LED_RING_7,EL_ON);

    /** infrared proximity sensors **/
    
    el_irps_is_calibrated = false;
    for(i=0;i<8;i++){
        el_irps_samples_NeutralNoise[i] = 0;
        el_irps_samples_NeutralReflection[i] = 0;
    }
    
    // get a random seed
    el_irps_working_mode = EL_IR_PROXIMITY_PASSIVE;
    el_enable_ir_proximity();
    w = 0;
    for(n=0;n<2;n++){
        el_sleep(50);
        for(i=0;i<8;i++){
            w += el_irps_samples_Ambient[i];
        }
    }
    el_disable_ir_proximity();

    el_random_set_seed(w);
    
    el_sleep(50);

    for(i=0;i<8;i++){
        el_irps_samples_Temp[i] = 0;
        el_random_uint16();
    }

    // calibrate reflection intensity
    el_irps_working_mode = EL_IR_PROXIMITY_PULSE;
    el_enable_ir_proximity();
    for(n=0;n<8;n++){
        el_sleep(50);
        for(i=0;i<8;i++){
            x = el_irps_samples_Mixed[i] - el_irps_samples_Ambient[i];
            el_irps_samples_Temp[i] += ((x>=0)? x:0);
        }
    }
    for(i=0;i<8;i++){
        x = el_irps_samples_Temp[i]/8;
        x *= 7;
        x /= 8;
        el_irps_samples_NeutralReflection[i] = x;
    }
    el_irps_samples_NeutralReflection[2] += 15;
    el_irps_samples_NeutralReflection[5] += 15;
    
    el_disable_ir_proximity();

    el_led_set(EL_LED_RING_1,EL_OFF);
    el_led_set(EL_LED_RING_3,EL_OFF);
    el_led_set(EL_LED_RING_5,EL_OFF);
    el_led_set(EL_LED_RING_7,EL_OFF);
    
    el_irps_is_calibrated = true;
    
}
