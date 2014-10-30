
#include "el.h"
#include "elu.h"

void booting_procedure01_selector_barrier();

void Process_LED_PatternA(void*arg);
void Process_LED_PatternB(void*arg);
void TimerCallback_LED_PatternC(void*arg);

//==============================================================================

int main(){
    el_handle T;

    el_initialization();
    
    booting_procedure01_selector_barrier();
    
    elu_printf("Hello World!\n");

    el_launch_process(Process_LED_PatternA,NULL);
    el_launch_process(Process_LED_PatternB,NULL);

    T = el_create_timer();
    el_timer_set_callback(T,TimerCallback_LED_PatternC,NULL);
    el_timer_start(T,1000);

    el_led_set(EL_LED_BODY,EL_ON);

    el_main_loop();

    return 0;
}

void TimerCallback_LED_PatternC(void*arg){
    //el_led_set(EL_LED_RING_4,EL_TOGGLE);
}

void Process_LED_PatternA(void*arg){
    while(1){
        el_led_set(EL_LED_BODY,EL_ON);
        el_process_wait(4800);
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
        i = i%8;
    }
    
}




void booting_procedure01_selector_barrier(){

    // do nothing until selector >= 2
    while(el_get_selector_value()<8){
        el_led_set(EL_LED_RING_5,EL_ON);
        el_sleep(100);
        el_led_set(EL_LED_RING_5,EL_OFF);
        el_sleep(900);
    }
    
}
