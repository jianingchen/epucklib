
#include <stdio.h>
#include "el_context.h"
#include "el.h"
#include "elu.h"

#define IR_REMOTE_KEY_START     53
#define IR_REMOTE_KEY_STOP      54
#define IR_REMOTE_KEY_PAUSE     41
#define IR_REMOTE_KEY_REW       37
#define IR_REMOTE_KEY_FF        38
#define IR_REMOTE_KEY_REC       55
#define IR_REMOTE_KEY_RESET     12
#define IR_REMOTE_KEY_MUTE      13
#define IR_REMOTE_KEY_PLUS      16
#define IR_REMOTE_KEY_MINUS     17
#define IR_REMOTE_KEY_NUM_0     0
#define IR_REMOTE_KEY_NUM_1     1
#define IR_REMOTE_KEY_NUM_2     2
#define IR_REMOTE_KEY_NUM_3     3
#define IR_REMOTE_KEY_NUM_4     4
#define IR_REMOTE_KEY_NUM_5     5
#define IR_REMOTE_KEY_NUM_6     6
#define IR_REMOTE_KEY_NUM_7     7
#define IR_REMOTE_KEY_NUM_8     8
#define IR_REMOTE_KEY_NUM_9     9
#define IR_REMOTE_KEY_INFO      18
#define IR_REMOTE_KEY_PRESET    14

volatile int CameraFrameCounter;
volatile int CameraFPS;
volatile bool CameraLoop;

void booting_procedure01_selector_barrier();
void booting_procedure02_led_pattern();
void booting_procedure03_say_hello();

EL_PROCESS Process_LED(void*data);
EL_PROCESS Process_TransmitCameraImage(void*data);
EL_PROCESS Process_AccelerometerFeedback(el_handle this_trigger);
EL_PROCESS Trigger_Accelerometer_Process(el_handle this_trigger);
void Callback_CountCameraFPS(void*arg);

//------------------------------------------------------------------------------

EL_PROCESS Process_DebugControl(void*data){
    el_camera_image *frame;
    char c;
    int i;
    int ir0,ir7;
    el_ir_proximity_data prox[8];
    el_int16 xyz[3];

    CameraLoop = false;
    /*
    elu_printf("ENTER YOUR PIN >>");
    elu_scanf("%d",&i);
    if(i==9527){
        elu_printf("PIN OK.\n");
    }else{
        elu_printf("PIN ERROR.\n");
    }
    */
    
    while(1){

        if(el_camera_is_frame_locked()==false){
            if(el_get_selector_value()>=12){
                if(CameraLoop==false){
                    CameraLoop = true;
                    elu_println("Camera Loop ON");
                    el_launch_process(Process_TransmitCameraImage,NULL);
                }
            }else{
                if(CameraLoop){
                    CameraLoop = false;
                    elu_println("Camera Loop OFF");
                }
            }
        }

        if(el_uart_get_char_counter(EL_UART_1) > 0){

            c = el_uart_get_char(EL_UART_1);
            switch(c){

            case 'f':
                el_led_set(EL_LED_FRONT,EL_TOGGLE);
                break;
                    
            case 'p':
                el_camera_lock_frame();
                frame = el_camera_frame();
                elu_println("[CAM,%d]",(int)el_camera_get_frame_counter());
                elu_println("DAT: %d, %d | FPS: %d",frame->Width,frame->Height,CameraFPS);
                elu_putchar('\n');
                el_camera_unlock_frame();
                break;
                
            case 'g':
                elu_println("[ACC]");
                el_accelerometer_get(EL_ACCELEROMETER_ONE,EL_ACCELERATION_ALL_3V,xyz);
                elu_println("%d\t%d\t%d",xyz[0],xyz[1],xyz[2]);
                elu_putchar('\n');
                break;
                
            case 'r':
                elu_println("[IR,%d]",(int)el_ir_proximity_get_counter());
                el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_ALL,EL_IR_ALL_3V,(el_int16*)prox);
                elu_printf("AMB:");
                for(i=0;i<8;i++){
                    elu_printf("\t%d",prox[i].Ambient);
                }
                elu_putchar('\n');
                elu_printf("REF:");
                for(i=0;i<8;i++){
                    elu_printf("\t%d",prox[i].Reflection);
                }
                elu_putchar('\n');
                break;

            case 'e':
                el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_0,EL_IR_REFLECTION,&ir0);
                el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_7,EL_IR_REFLECTION,&ir7);
                elu_println("%d,%d",ir0,ir7);
                break;

            case 'c':
                for(i=0;i<8;i++){
                    elu_printf("IRNR:\t%d",el_irps_samples_NeutralReflection[i]);
                }
                elu_putchar('\n');
                break;

            case 'w':
                el_set_wheel_speed(1000,1000);
                break;

            case 'a':
                el_set_wheel_speed(-250,250);
                break;

            case 's':
                el_set_wheel_speed(-500,-500);
                break;

            case 'd':
                el_set_wheel_speed(250,-250);
                break;
                
            case ' ':
                el_set_wheel_speed(0,0);
                break;
                
            case 'T':
                el_config_stepper_motor_list()->UseAcceleration = true;
                el_config_stepper_motor(el_config_stepper_motor_list());
                elu_println("MOTOR ACC ON");
                break;
                
            case 't':
                el_config_stepper_motor_list()->UseAcceleration = false;
                el_config_stepper_motor(el_config_stepper_motor_list());
                elu_println("MOTOR ACC OFF");
                break;


            }
        }

        el_process_cooperate();
    }
}

int main(int argc,char*argv[]){
    int i,j;
    el_camera_param *CameraSetting;
    el_mct k;
    el_handle T;
    el_handle trg;

    el_stepper_motor_param *StepperMotorSetting;
    StepperMotorSetting = el_config_stepper_motor_list();
    StepperMotorSetting->UseAcceleration = true;
    StepperMotorSetting->AccelerationRate = 2000;
    el_config_stepper_motor(StepperMotorSetting);

    el_initialization();

    el_calibrate_sensors();

    booting_procedure01_selector_barrier();// very essential
    booting_procedure02_led_pattern();// recommended
    booting_procedure03_say_hello();// recommended


#if 0
    p1 = malloc(16);
    elu_printf("%d,%d\n",p0,p1);
    elu_printf("%d,%d\n",&i,&j);
#endif

    el_config_ir_proximity(el_config_ir_proximity_list());
    el_config_stepper_motor(el_config_stepper_motor_list());

    k = el_get_masterclock();
    CameraSetting = el_config_camera_list();
    CameraSetting->BlueGain = 1.0f;
    el_config_camera(CameraSetting);
    k = el_get_masterclock() - k;
    elu_println("%lu",k);
    
    CameraFrameCounter = 0;
    CameraFPS = 0;

    T = el_create_timer();
    el_timer_set_callback(T,Callback_CountCameraFPS,NULL);
    el_timer_start(T,1000);

    el_launch_process(Process_DebugControl,NULL);
    el_launch_process(Process_LED,NULL);

    trg = el_create_trigger();
    el_trigger_set_process(trg,Trigger_Accelerometer_Process);
    el_trigger_set_event(trg,EL_EVENT_ACCELEROMETER_UPDATE);

    el_enable_ir_proximity();
    el_enable_camera();
    el_enable_accelerometer();

    el_main_loop();
    
    return 0;
}

//------------------------------------------------------------------------------

EL_PROCESS Process_LED(void*data){

    while(1){

        el_led_set(EL_LED_RING_2,1);
        el_led_set(EL_LED_RING_6,0);
        el_process_wait(500);

        el_led_set(EL_LED_RING_2,0);
        el_led_set(EL_LED_RING_6,1);
        el_process_wait(500);

    }

}

EL_PROCESS Process_TransmitCameraImage(void*data){
    int X,Y,i;
    el_camera_image *frame;
    char str[80];
    char *p;

    el_process_wait(100);

    while(CameraLoop){

        el_led_set(EL_LED_RING_0,1);

        el_camera_lock_frame();
        frame = el_camera_frame();
        i = el_camera_get_frame_counter();

        p = str;
        //p += elu_snprintf(p,(80 - (p - str)),"pointer: %p\n",frame);
        p += elu_snprintf(p,(80 - (p - str)),"<p>\n");
        p += elu_snprintf(p,(80 - (p - str)),"NUM: %d\n",i);
        p += elu_snprintf(p,(80 - (p - str)),"DIM: %d,%d\n",EL_CAMERA_FRAME_DIM_X,EL_CAMERA_FRAME_DIM_Y);
        p += elu_snprintf(p,(80 - (p - str)),"LEN: %d,%d\n",frame->Width,frame->Height);
        while(el_uart_is_sending(EL_UART_1)) el_process_cooperate();
        el_uart_send_string(EL_UART_1,str);
        for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){
            X = 0;
            while(X < EL_CAMERA_FRAME_DIM_X){
                p = str;
                for(i=0;i<10;i++){
                    p += elu_snprintf(p,80,"%4.4X",frame->Data[Y][X]);
                    X++;
                }
                if(X==EL_CAMERA_FRAME_DIM_X){
                    p += elu_snprintf(p,80,"\n");
                    if(Y==(EL_CAMERA_FRAME_DIM_Y - 1)){
                        p += elu_snprintf(p,80,"</p>\n");
                    }
                }
                while(el_uart_is_sending(EL_UART_1)) el_process_cooperate();
                el_uart_send_string(EL_UART_1,str);
            }
        }

        el_camera_unlock_frame();

        el_led_set(EL_LED_RING_0,0);

        el_process_wait(1000);

    }

}

EL_PROCESS Trigger_Accelerometer_Process(el_handle this_trigger){
    static int previous_xyz[3] = {0,0,0};
    const long threshold_s = 300L*300L;
    long magnitude_s;// _s means squared;
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
        if(magnitude_s > threshold_s){
            el_led_set(EL_LED_BODY,EL_ON);
            el_process_wait(200);
            el_led_set(EL_LED_BODY,EL_OFF);
            el_process_wait(200);
            // 400 ms past, need to refresh values
            el_accelerometer_get(EL_ACCELEROMETER_ONE,EL_ACCELERATION_ALL_3V,xyz);
        }
    }
    previous_xyz[0] = xyz[0];
    previous_xyz[1] = xyz[1];
    previous_xyz[2] = xyz[2];

    el_trigger_enable(this_trigger);

}

void Callback_CountCameraFPS(void*arg){
    unsigned int c = el_camera_get_frame_counter();
    unsigned int d = c - CameraFrameCounter;
    CameraFrameCounter = c;
    CameraFPS = d;
}

//------------------------------------------------------------------------------

void booting_procedure01_selector_barrier(){
    // do nothing until selector >= 4
    while(el_get_selector_value()<4){
        el_led_set(EL_LED_RING_5,1);
        el_sleep(125);
        el_led_set(EL_LED_RING_5,0);
        el_sleep(3000 - 125);
    }

}

void booting_procedure02_led_pattern(){
    
    el_led_set(EL_LED_RING_ALL,EL_ON);
    el_sleep(500);
    el_led_set(EL_LED_RING_ALL,EL_OFF);
    el_sleep(500);
    
}

void booting_procedure03_say_hello(){
    el_uart_send_string(EL_UART_1,"Hello World!\n");
    elu_printf("This is e-puck!\r\n");
}
