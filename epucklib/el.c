
#include "el_context.h"
#include "el_interrupt.h"
#include "el.h"

void el_initialization(){

    e_init_port();
    e_init_uart1();
    e_init_uart2();
    e_i2cp_init();

    el_init_masterclock();
    el_init_timers();
    el_init_triggers();// including cmt

    el_init_stepper_motor();
    el_init_ir_receiver();
    el_init_accelerometer();
    el_init_ir_proximity();
    el_init_camera();

    el_init_interrupt_INT0();
    el_init_interrupt_ADC();
    el_init_interrupt_T1();
    el_init_interrupt_T2();
    el_init_interrupt_T3();
    el_init_interrupt_T4();
    el_init_interrupt_T5();

}

void el_main_loop(){

    el_timer_mck = el_get_masterclock();
    el_trigger_mck = el_get_masterclock();

    while(1){

        el_routine_timers();
        el_routine_triggers();

    }

}

void el_sleep(el_time time_ms){
    el_mct k;
    
    if(el_is_in_cmt_process){
        return;
    }
    
    k = el_get_masterclock() + EL_TIME_TO_MTK(time_ms);;
    while(el_get_masterclock() < k){
        NOP();
    }
}

void el_reset(){
    RESET();
}

int el_get_selector_value(){
    return (SELECTOR3<<3)|(SELECTOR2<<2)|(SELECTOR1<<1)|SELECTOR0;
}
