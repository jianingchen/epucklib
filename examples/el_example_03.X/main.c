/*

embedded system library for e-puck - example 3

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
void TimerCallback_MotionAutoStop(void*arg);
void TimerCallback_MeasureCameraFPS(void*arg);
void Process_ConsoleLoop(void*arg);


el_ir_proximity_data ProximitySensor[8];
volatile el_uint32 CameraFrameCounter;
volatile el_int16 CameraFrameRate;
el_handle CameraFrameRateTimer;
el_handle MotionTimer;


int main(int argc,char*argv[]){

    el_initialization();
    el_calibrate_sensors();
    el_uart_use_reset_code(true,128);
    
    /*
     * Put the robot in silence when the selector is in 0~3. 
     */
    BootingProcedure01_SelectorBarrier();
    
    
    /*
     * The values below are same as the default configurations; they are placed
     * here just for demonstration. Note, when the automatic mode is used, the
     * frame rate will be varied depending on the lighting condition of the
     * environment. 
     */
    el_config_camera_list()->ExposureMode = EL_AUTOMATIC;
    el_config_camera_list()->AutoWhiteBalance = true;
    el_config_camera_list()->AutoDigitalGain = true;
    el_config_camera(el_config_camera_list());
    
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
    const int MOTION_TIME = 600;
    char c;
    int i;

    elu_printf("EL_EXAMPLE_03\n");
    
    el_config_stepper_motor_list()->UseAcceleration = true;
    el_config_stepper_motor_list()->AccelerationRate = 2000;
    el_config_stepper_motor(el_config_stepper_motor_list());
    
    el_enable_camera();
    el_enable_ir_proximity();
    el_enable_stepper_motor();
    
    el_process_wait(500);// wait for 500 ms

    // setup a timer to measure framerate of the camera
    CameraFrameRateTimer = el_create_timer();
    el_timer_set_callback(CameraFrameRateTimer,TimerCallback_MeasureCameraFPS,NULL);
    el_timer_start(CameraFrameRateTimer,1000);// 1000 milliseconds

    // setup a timer to stop moving automatically after last motion command
    MotionTimer = el_create_timer();
    el_timer_set_perodic(MotionTimer,false);
    el_timer_set_callback(MotionTimer,TimerCallback_MotionAutoStop,NULL);

    el_uart_flush_char(EL_UART_1);
    el_led_set(EL_LED_BODY,EL_ON);
    
    while(1){

        do{
            el_process_cooperate();
        }while(el_uart_get_char_counter(EL_UART_1)==0);
        
        c = el_uart_get_char(EL_UART_1);
        
        switch(c){

        case 'H':
        case 'h':// list available commands
            elu_println("<HELP>");
            elu_println("Key\tAction");
            elu_println("'1'\ttoggle front led");
            elu_println("'2'\ttoggle body led");
            elu_println("'r'\treport ir proximity sensor outputs");
            elu_println("'y'\treport camera framerate");
            elu_println("'t'\ttransmit camera image");
            elu_println("'w'\tmove forward");
            elu_println("'a'\tspin left");
            elu_println("'s'\tmove backward");
            elu_println("'d'\tspin right");
            elu_println("'q'\tturn left");
            elu_println("'e'\tturn right");
            elu_println("' '\tstop");
            break;

        case 'f':
        case '1':
            el_led_set(EL_LED_FRONT,EL_TOGGLE);
            break;

        case '2':
            el_led_set(EL_LED_BODY,EL_TOGGLE);
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

        case 'y':// report camera framerate
            elu_println("[CAM]\nFPS:\t%d\tNUM:\t%lu",CameraFrameRate,CameraFrameCounter);
            break;

        case 't':
            el_camera_lock_frame();
            el_print_camera_image(el_camera_frame(),el_camera_get_frame_counter());
            el_camera_unlock_frame();
            elu_putchar('\n');
            break;

        case 'w':
            el_set_wheel_speed(800,800);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case 'a':
            el_set_wheel_speed(-200,200);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case 's':
            el_set_wheel_speed(-500,-500);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case 'd':
            el_set_wheel_speed(200,-200);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case 'q':
            el_set_wheel_speed(500,800);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case 'e':
            el_set_wheel_speed(800,500);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;

        case ' ':
            el_set_wheel_speed(0,0);
            el_timer_start(MotionTimer,MOTION_TIME);
            break;
        }

    }

}

void TimerCallback_MotionAutoStop(void*arg){
    el_set_wheel_speed(0,0);
}

void TimerCallback_MeasureCameraFPS(void*args){
    el_uint32 c = el_camera_get_frame_counter();
    el_int16 d = c - CameraFrameCounter;
    CameraFrameCounter = c;
    CameraFrameRate = d;
}
