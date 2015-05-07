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

void BootingProcedure01_SelectorBarrier();
void Process_ConsoleLoop(void*arg);
void Process_IrReceiverLoop(void*arg);
void Trigger_Accelerometer_Process(void*arg);


el_handle Trigger_Accelerometer;


int main(void){

    el_initialization();

    /*
     * Calibrate the sensors and generate a random seed.
     */
    el_calibrate_sensors();

    /*
     * This is to let the robot automaticaly reset when TinyBootloader
     * attemps to write a new HEX, so you dont need to touch the reset
     * button.
     * To achieve this, TinyBootloader also needs to be configured:
     * in "Options" tab, set "Codes to send first" to 128.
     */
    el_uart_use_reset_code(true,128);

    /*
     * Put the robot in silence when the selector is in 0~3. 
     */
    BootingProcedure01_SelectorBarrier();
    
    
    elu_printf("EL_EXAMPLE_02\n");

    Trigger_Accelerometer = el_create_trigger();
    el_trigger_set_event(Trigger_Accelerometer,EL_EVENT_ACCELEROMETER_UPDATE);
    el_trigger_set_process(Trigger_Accelerometer,Trigger_Accelerometer_Process);
    
    el_launch_process(Process_ConsoleLoop,NULL);
    el_launch_process(Process_IrReceiverLoop,NULL);

    el_enable_ir_receiver();
    el_enable_accelerometer();// Tips: never forget to enable something

    el_main_loop();
    
    // Tips: everything 'create'ed by user's code must be 'delete'ed by user's code
    el_delete_trigger(Trigger_Accelerometer);
    
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
    el_int16 xyz[3];

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

        case 'g':// report accelerometer outputs
            elu_println("[ACC]");
            el_accelerometer_get(EL_ACCELEROMETER_ONE,EL_ACCELERATION_ALL_3V,xyz);
            elu_println("%d\t%d\t%d",xyz[0],xyz[1],xyz[2]);
            break;

        }

    }

}

void Process_IrReceiverLoop(void*arg){
    int check;
    int address;
    int key_value;

    while(1){

        // ir receiver requires a reset before a potential income
        el_ir_receiver_reset();

        // wait for a new income ('check' becoming either 0 or 1)
        do{
            el_process_cooperate();
            check = el_ir_receiver_get_check();
        }while( (check != 0)&&(check != 1) );

        // grab the data
        address = el_ir_receiver_get_address();
        key_value = el_ir_receiver_get_data();

        // display the received data
        elu_println("[IRRC]");
        elu_println("%d\t%d\t%d",check,address,key_value);

    }

}

void Trigger_Accelerometer_Process(el_handle this_trigger){
    static int previous_xyz[3] = {0,0,0};
    const long threshold_s = 330L*330L;
    long magnitude_s;// must be a long int otherwise may overflow
    int xyz[3];
    int dx,dy,dz;

    el_accelerometer_get(EL_ACCELEROMETER_ONE,EL_ACCELERATION_ALL_3V,xyz);

    if(previous_xyz[0]!=0){
        
        dx = xyz[0] - previous_xyz[0];
        dy = xyz[1] - previous_xyz[1];
        dz = xyz[2] - previous_xyz[2];

        magnitude_s = (long)dx*dx;
        magnitude_s += (long)dy*dy;
        magnitude_s += (long)dz*dz;
        
        /*
         * Tip:
         * If the mathmatical validity allows, keeping all values in square
         * can save the computation cost of a square root. 
         */
        if(magnitude_s > threshold_s){

            // flash all ring LEDs if a shake or impact is detected

            el_led_set(EL_LED_RING_ALL,EL_ON);
            el_process_wait(200);

            el_led_set(EL_LED_RING_ALL,EL_OFF);
            el_process_wait(200);

            // 400 ms past, need to refresh values to be used as previous values in next round
            el_accelerometer_get(EL_ACCELEROMETER_ONE,EL_ACCELERATION_ALL_3V,xyz);

        }

    }

    previous_xyz[0] = xyz[0];
    previous_xyz[1] = xyz[1];
    previous_xyz[2] = xyz[2];

    /*
     * once the process of a trigger is launched, the trigger is disabled
     * automatically. This is to prevent nested launches of process when the
     * event of the trigger is very frequent and there is no trigger condition.
     * So re-enabling a trigger is often needed at the end of a trigger process.
     */
    el_trigger_enable(this_trigger);

}
