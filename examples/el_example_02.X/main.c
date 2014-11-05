
#include <el.h>
#include <elu.h>

#include "ServiceTrigger_CameraImageProcessing.h"
#include "TaskTrigger_ObjectFollowing.h"

void booting_procedure01_selector_barrier();

void Process_ConsoleLoop(void*arg);

//==============================================================================

int main(int argc,char*argv[]){

    el_initialization();
    el_calibrate_sensors();

    booting_procedure01_selector_barrier();

    el_launch_process(Process_ConsoleLoop,NULL);

    el_main_loop();

    return 0;
}

void Process_ConsoleLoop(void*arg){
    char c;
    int ir_ref[8];

    elu_printf("EL_EXAMPLE_02\n");

    Trigger_CameraImageProcessing_Setup();
    Trigger_ObjectFollowing_Setup();

    el_config_ir_proximity(EL_WORKING_MODE,EL_IR_PROXIMITY_PULSE);
    el_enable_ir_proximity();
    
    el_config_stepper_motor(EL_SPEED_ACC_ENABLE,true);
    el_config_stepper_motor(EL_SPEED_ACC_LINEAR_TERM,2000);
    el_enable_stepper_motor();

    el_enable_camera();
    
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
                el_ir_proximity_get_all(EL_IR_REFLECTION,ir_ref);
                elu_printf("%d\t%d\t%d\t%d\t",ir_ref[0],ir_ref[1],ir_ref[2],ir_ref[3]);
                elu_printf("%d\t%d\t%d\t%d\n\n",ir_ref[4],ir_ref[5],ir_ref[6],ir_ref[7]);
                break;

            case 'p':// report image processing result
                elu_printf("<IMG>\n");
                elu_printf("MASS:\t%d\t%d\t%d\n",IMG_RedMass,IMG_GreenMass,IMG_BlueMass);
                elu_printf("BIAS:\t%d\t%d\t%d\n\n",IMG_RedBias,IMG_GreenBias,IMG_BlueBias);
                break;

            case '1':
                TT_ObjectColor = TT_OBJECT_COLOR_RED;
                elu_printf("FOLLOW RED\n");
                break;

            case '2':
                TT_ObjectColor = TT_OBJECT_COLOR_GREEN;
                elu_printf("FOLLOW GREEN\n");
                break;

            case '3':
                TT_ObjectColor = TT_OBJECT_COLOR_BLUE;
                elu_printf("FOLLOW BLUE\n");
                break;
                
            }

        }

        el_process_cooperate();
    }

}

//==============================================================================

void booting_procedure01_selector_barrier(){
    // do nothing until selector >= 4
    while(el_get_selector_value()<4){
        el_led_set(EL_LED_RING_5,1);
        el_sleep(125);
        el_led_set(EL_LED_RING_5,0);
        el_sleep(875);
    }
}
