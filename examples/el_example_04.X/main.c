/*

embedded system library for e-puck - example 4

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

#include <el.h>
#include <elu.h>

#include "Task_ObjectFollowing.h"

void BootingProcedure01_SelectorBarrier();
void TimerCallback_MeasureCameraFPS(void*arg);
void Process_ConsoleLoop(void*arg);


el_ir_proximity_data ProximitySensor[8];


int main(void){

    el_initialization();
    el_calibrate_sensors();
    el_uart_use_reset_code(true,128);
    
    BootingProcedure01_SelectorBarrier();
    
    el_launch_process(Process_ConsoleLoop,NULL);

    el_main_loop();

    return 0;
}

void BootingProcedure01_SelectorBarrier(){
    while(el_get_selector_value()<4){
        el_led_set(EL_LED_RING_5,1);
        el_sleep(125);
        el_led_set(EL_LED_RING_5,0);
        el_sleep(875);
    }
}

void Process_ConsoleLoop(void*arg){
    char c;
    int i;

    elu_printf("EL_EXAMPLE_04\n");

    // setup the task
    Task_ObjectFollowing_Setup();
    
    el_process_wait(500);// wait for 500 ms

    el_uart_flush_char(EL_UART_1);

    while(1){

        do{
            el_process_cooperate();
        }while(el_uart_get_char_counter(EL_UART_1)==0);

        c = el_uart_get_char(EL_UART_1);
        
        switch(c){

        case 'f':
            el_led_set(EL_LED_FRONT,EL_TOGGLE);
            break;

        case 'r':// report ir proximity sensor outputs
            elu_println("<IR>");
            el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_ALL,EL_IR_ALL_3V,(el_int16*)ProximitySensor);
            elu_printf("AMB:");
            for(i=0;i<8;i++){
                elu_printf("\t%d",ProximitySensor[i].Ambient);
            }
            elu_putchar('\n');
            elu_printf("REF:");
            for(i=0;i<8;i++){
                elu_printf("\t%d",ProximitySensor[i].Reflection);
            }
            elu_putchar('\n');
            break;

        case 'p':// report image processing result
            elu_println("<IMG>");
            elu_println("MASS:\t%d\t%d\t%d",vision_mass_red,vision_mass_green,vision_mass_blue);
            elu_println("BIAS:\t%d\t%d\t%d",vision_bias_red,vision_bias_green,vision_bias_blue);
            elu_putchar('\n');
            break;

        case '1':
            Task_ObjectColor = TASK_OBJECT_RED;
            elu_println("FOLLOW RED");
            break;

        case '2':
            Task_ObjectColor = TASK_OBJECT_GREEN;
            elu_println("FOLLOW GREEN");
            break;

        case '3':
            Task_ObjectColor = TASK_OBJECT_BLUE;
            elu_println("FOLLOW BLUE");
            break;

        }

    }

    Task_ObjectFollowing_Clear();

}
