
#include "el_context.h"
#include "el_initialization.h"

void el_initialization(){
    
    e_init_port();
    e_init_uart1();
    e_init_uart2();
    e_i2cp_init();
    
    el_init_timers();
    el_init_stepper_motor();
    el_init_camera();
    el_init_ir_receiver();
    el_init_ir_proximity();
    
    el_init_triggers();
    
    el_init_interrupt_INT0();
    el_init_interrupt_ADC();
    el_init_interrupt_T1();
    el_init_interrupt_T2();
    el_init_interrupt_T3();
    el_init_interrupt_T4();
    el_init_interrupt_T5();

}

void el_main_loop(){

    el_timers_mck = el_get_masterclock();
    el_triggers_mck = el_get_masterclock();

    while(1){
        
        el_routine_timers();
        el_routine_triggers();
        
    }

}

void el_polled_wait(el_time time_ms){
    int i;
    if(el_is_in_cmt_process){
        return;
    }
    while(time_ms--){
        i = 1192;
        while(i--) NOP();
    }
}

void el_reset_chip(){
    RESET();
}

int el_get_selector_value(){
    return (SELECTOR3<<3)|(SELECTOR2<<2)|(SELECTOR1<<1)|SELECTOR0;
}


#define EL_DEAL_WITH_IT(v)\
    if(action==EL_TOGGLE){\
        v = v^1;\
    }else{\
        v = action;\
    }

void el_set_led(int which,int action){
    
    switch(which){

        case EL_RING_LED_0:
        EL_DEAL_WITH_IT(LED0);
        break;

        case EL_RING_LED_1:
        EL_DEAL_WITH_IT(LED1);
        break;

        case EL_RING_LED_2:
        EL_DEAL_WITH_IT(LED2);
        break;

        case EL_RING_LED_3:
        EL_DEAL_WITH_IT(LED3);
        break;

        case EL_RING_LED_4:
        EL_DEAL_WITH_IT(LED4);
        break;

        case EL_RING_LED_5:
        EL_DEAL_WITH_IT(LED5);
        break;

        case EL_RING_LED_6:
        EL_DEAL_WITH_IT(LED6);
        break;

        case EL_RING_LED_7:
        EL_DEAL_WITH_IT(LED7);
        break;

        case EL_RING_LED_ALL:
        if(action==EL_TOGGLE){
            LED0 = LED0^1;
            LED1 = LED1^1;
            LED2 = LED2^1;
            LED3 = LED3^1;
            LED4 = LED4^1;
            LED5 = LED5^1;
            LED6 = LED6^1;
            LED7 = LED7^1;
        }else{
            LED0 = action;
            LED1 = action;
            LED2 = action;
            LED3 = action;
            LED4 = action;
            LED5 = action;
            LED6 = action;
            LED7 = action;
        }
        break;

        case EL_BODY_LED:
        if(action==EL_TOGGLE){
            BODY_LED = BODY_LED^1;
        }else{
            BODY_LED = action;
        }
        break;

        case EL_FRONT_LED:
        if(action==EL_TOGGLE){
            FRONT_LED = FRONT_LED^1;
        }else{
            FRONT_LED = action;
        }
        break;
        
        default:
        break;
    }
}

#undef EL_DEAL_WITH_IT
