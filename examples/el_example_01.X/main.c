/*

embedded system library for e-puck - example 1

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

#include "el.h"
#include "elu.h"

void booting_procedure01_selector_barrier();

void Process_LED_PatternA(void*arg);
void Process_LED_PatternB(void*arg);
void Process_LED_Control(void*arg);

void TimerCallback_ReportTime(void*arg);

//==============================================================================

int main(){
    el_handle T;

    el_initialization();

    // do not transmit or do anything until the selector allows
    booting_procedure01_selector_barrier();

    // see documentation for details about "elu_printf" and the UART module
    elu_printf("Hello World! This is e-puck!\n");

    // setup some process, which will be concurrenly executed in "el_main_loop()"
    el_launch_process(Process_LED_PatternA,NULL);
    el_launch_process(Process_LED_PatternB,NULL);
    el_launch_process(Process_LED_Control,NULL);

    // setup a timer
    T = el_create_timer();
    el_timer_set_callback(T,TimerCallback_ReportTime,NULL);
    el_timer_start(T,1000);// 1000 milliseconds

    el_led_set(EL_LED_BODY,EL_ON);

    elu_printf("setup finished\n");

    // this it the loop to run everything, including Timer, Process and Trigger. 
    el_main_loop();

    return 0;
}

void Process_LED_PatternA(void*arg){
    
    while(1){

        el_led_set(EL_LED_BODY,EL_ON);
        
        el_process_wait(4800);// "el_process_wait" works only in a process

        el_led_set(EL_LED_BODY,EL_OFF);

        el_process_wait(200);

    }

}

void Process_LED_PatternB(void*arg){
    int i = 0;

    while(1){

        el_led_set(EL_LED_RING_0 + i,EL_ON);

        el_process_wait(125);

        el_led_set(EL_LED_RING_0 + i,EL_OFF);
        
        i++;
        i %= 8;
    }
    
}

void Process_LED_Control(void*arg){
    int i = 0;
    char c;

    while(1){

        // wait until there is some char
        while(el_uart_get_char_counter(EL_UART_1)==0){
            el_process_cooperate();// important for a polling mechnism like this.
        }

        c = el_uart_get_char(EL_UART_1);
        if(c=='f'){
            el_led_set(EL_LED_FRONT,EL_TOGGLE);
        }
        
    }
    
}

void TimerCallback_ReportTime(void*arg){
    el_handle T = el_timer_callback_get_handle();

    elu_printf("%d\n",el_timer_get_rounds(T));
    
    /*
    Unlike a Process, a Timer Callback cannot include any time delay or polling.
    For details, check the documentation.
    */
}


//==============================================================================

void booting_procedure01_selector_barrier(){

    // do nothing until selector >= 4
    while(el_get_selector_value()<4){
        el_led_set(EL_LED_RING_5,EL_ON);
        el_sleep(100);// note: "el_sleep" does not work in a process
        el_led_set(EL_LED_RING_5,EL_OFF);
        el_sleep(900);
    }
    
}
