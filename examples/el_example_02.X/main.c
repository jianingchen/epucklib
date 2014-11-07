/*

embedded system library for e-puck - example 2

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

#include "ServiceTrigger_CameraImageProcessing.h"
#include "TaskTrigger_ObjectFollowing.h"

void booting_procedure01_selector_barrier();
void Process_ConsoleLoop(void*arg);

/*
 * Big-sized variables should be declared as global variables.
 */
el_ir_proximity_data ProximitySensor[8];



int main(int argc,char*argv[]){

    el_initialization();
    el_calibrate_sensors();

    /*
     * Put the robot in silence when the selector is in 0~3. 
     */
    booting_procedure01_selector_barrier();
    
    
    /*
     * The values below are same as the default configurations; they are placed
     * here just for demonstration.
     */
    el_config_camera_list()->ExposureMode = EL_AUTOMATIC;
    el_config_camera_list()->AutoWhiteBalance = true;
    el_config_camera_list()->AutoDigitalGain = true;
    el_config_camera(el_config_camera_list());

    el_launch_process(Process_ConsoleLoop,NULL);

    el_main_loop();

    return 0;
}

void booting_procedure01_selector_barrier(){
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

    elu_printf("EL_EXAMPLE_02\n");

    Trigger_CameraImageProcessing_Setup();
    Trigger_ObjectFollowing_Setup();
    
    el_config_stepper_motor_list()->UseAcceleration = true;
    el_config_stepper_motor_list()->AccelerationRate = 2000;
    el_config_stepper_motor(el_config_stepper_motor_list());
    
    el_enable_camera();
    el_enable_ir_proximity();
    el_enable_stepper_motor();
    
    el_process_wait(500);// wait for 500 ms

    el_led_set(EL_LED_BODY,EL_ON);
    el_uart_flush_char(EL_UART_1);

    while(1){

        if(el_uart_get_char_counter(EL_UART_1) > 0){

            c = el_uart_get_char(EL_UART_1);

            // Note: printf functions for this chip do not support float.

            switch(c){

            case 'f':
                el_led_set(EL_LED_FRONT,EL_TOGGLE);
                break;

            case 'r':// report ir proximity sensor outputs
                elu_printf("<IR>\n");
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
                elu_println("<IMG>\n");
                elu_println("MASS:\t%d\t%d\t%d",IMG_RedMass,IMG_GreenMass,IMG_BlueMass);
                elu_println("BIAS:\t%d\t%d\t%d",IMG_RedBias,IMG_GreenBias,IMG_BlueBias);
                elu_putchar('\n');
                break;

            case '1':
                TT_ObjectColor = TT_OBJECT_COLOR_RED;
                elu_println("FOLLOW RED");
                break;

            case '2':
                TT_ObjectColor = TT_OBJECT_COLOR_GREEN;
                elu_println("FOLLOW GREEN");
                break;

            case '3':
                TT_ObjectColor = TT_OBJECT_COLOR_BLUE;
                elu_println("FOLLOW BLUE");
                break;
                
            }

        }

        el_process_cooperate();
    }

}
