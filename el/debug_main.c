
#include <stdio.h>
#include "el_context.h"
#include "el.h"
#include "elu.h"

volatile int CameraFrameCounter;
volatile int CameraFPS;
volatile bool CameraLoop;


void booting_procedure01_selector_barrier();
void booting_procedure02_led_pattern();
void booting_procedure03_say_hello();

EL_PROCESS Routine_LED(void*data){

    while(1){

        el_led_set(EL_LED_BODY,1);
        el_process_wait(500);

        el_led_set(EL_LED_BODY,0);
        el_process_wait(500);

    }

}

EL_PROCESS Routine_TransmitCameraImage(void*data){
    int X,Y,i;
    el_camera_image *frame;
    char str[80];
    char *p;

    el_process_wait(100);

    while(CameraLoop){

        el_led_set(EL_LED_RING_0,1);

        el_camera_lock_frame();
        frame = el_camera_get_frame();

        p = str;
        //p += elu_snprintf(p,(80 - (p - str)),"pointer: %p\n",frame);
        p += elu_snprintf(p,(80 - (p - str)),"<p>\n");
        p += elu_snprintf(p,(80 - (p - str)),"FPS: %d\n",CameraFPS);
        p += elu_snprintf(p,(80 - (p - str)),"FMT: %d\n",565);
        p += elu_snprintf(p,(80 - (p - str)),"RAW: %d,%d\n",frame->width,frame->height);
        p += elu_snprintf(p,(80 - (p - str)),"DAT: %d,%d\n",EL_CAMERA_FRAME_BUFFER_WIDTH,EL_CAMERA_FRAME_BUFFER_HEIGHT);
        while(el_uart_is_sending(EL_UART_1)) el_process_cooperate();
        el_uart_send_string(EL_UART_1,str);
        for(Y=0;Y<EL_CAMERA_FRAME_BUFFER_HEIGHT;Y++){
            X = 0;
            while(X<EL_CAMERA_FRAME_BUFFER_WIDTH){
                p = str;
                for(i=0;i<10;i++){
                    p += elu_snprintf(p,80,"%4.4X",frame->data[Y][X]);
                    X++;
                }
                if(X==EL_CAMERA_FRAME_BUFFER_WIDTH){
                    p += elu_snprintf(p,80,"\n");
                    if(Y==(EL_CAMERA_FRAME_BUFFER_HEIGHT - 1)){
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

EL_PROCESS Process_AccelerometerFeedback(el_handle this_trigger){
    int v;

    v = el_accelerometer_get(EL_ACCELEROMETER_GET_X);
    elu_printf("%d,",v);
    
    v = el_accelerometer_get(EL_ACCELEROMETER_GET_Y);
    elu_printf("%d,",v);

    v = el_accelerometer_get(EL_ACCELEROMETER_GET_Z);
    elu_printf("%d,",v);

    elu_printf("\n");

}

EL_PROCESS Trigger_Accelerometer_Process(el_handle this_trigger){
    static int previous_xyz[3] = {0,0,0};
    const long threshold_s = 250L*250L;
    long magnitude_s;// _s means squared;
    int xyz[3];
    int dx,dy,dz;
    
    el_accelerometer_get_all(xyz);
    if(previous_xyz[0]!=0){
        dx = xyz[0] - previous_xyz[0];
        dy = xyz[1] - previous_xyz[1];
        dz = xyz[2] - previous_xyz[2];
        magnitude_s = (long)dx*dx;
        magnitude_s += (long)dy*dy;
        magnitude_s += (long)dz*dz;
        if(magnitude_s > threshold_s){
            el_led_set(EL_LED_RING_4,EL_ON);
            el_process_wait(200);
            el_led_set(EL_LED_RING_4,EL_OFF);
            el_process_wait(200);
            // 400 ms past, need to refresh values
            el_accelerometer_get_all(xyz);
        }
    }
    previous_xyz[0] = xyz[0];
    previous_xyz[1] = xyz[1];
    previous_xyz[2] = xyz[2];
    
    el_trigger_enable(this_trigger);
    
}

EL_PROCESS Routine_DebugControl(void*data){
    el_camera_image *frame;
    char c;
    int i;
    int value[8];

    CameraLoop = false;

    while(1){

        if(el_camera_is_frame_locked()==false){
            if(el_get_selector_value()>=12){
                if(CameraLoop==false){
                    CameraLoop = true;
                    elu_printf("Camera Loop ON\n");
                    el_launch_process(Routine_TransmitCameraImage,NULL);
                }
            }else{
                if(CameraLoop){
                    CameraLoop = false;
                    elu_printf("Camera Loop OFF\n");
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
                frame = el_camera_get_frame();
                elu_printf("DAT: %d, %d | FPS: %d\n",frame->width,frame->height,CameraFPS);
                el_camera_unlock_frame();
                break;
                
            case 'r':
                elu_printf("%d:",el_ir_proximity_get_counter());
                el_ir_proximity_get_all(EL_IR_REFLECTION,value);
                for(i=0;i<8;i++){
                    elu_printf("\t%d",value[i]);
                }
                elu_printf("\n");
                break;

            case 'x':
                elu_printf("IREA:\t%d\n",el_irps_environment_ambient);
                break;

            case 'c':
                for(i=0;i<8;i++){
                    elu_printf("IRNR:\t%d",el_irps_samples_NeutralReflection[i]);
                }
                elu_printf("\n");
                break;
            }
        }

        el_process_cooperate();
    }
}

void Callback_CountCameraFPS(void*arg){
    unsigned int c = el_camera_get_frame_counter();
    unsigned int d = c - CameraFrameCounter;
    CameraFrameCounter = c;
    CameraFPS = d;// a filter to make value stable
}

int main(int argc,char*argv[]){
    int i,j;
    void *p0,*p1;
    el_handle T;
    el_handle trg;

    p0 = malloc(16);

    el_initialization();

    booting_procedure01_selector_barrier();// very essential
    booting_procedure02_led_pattern();// recommended
    booting_procedure03_say_hello();// recommended

    el_calibrate_sensors();

#if 0
    p1 = malloc(16);
    elu_printf("%d,%d\n",p0,p1);
    elu_printf("%d,%d\n",&i,&j);
#endif

    CameraFrameCounter = 0;
    CameraFPS = 0;

    T = el_create_timer();
    el_timer_set_callback(T,Callback_CountCameraFPS,NULL);
    el_timer_start(T,1000);


    el_config_ir_proximity(EL_IR_PROXIMITY_MODE_PULSE);
    el_enable_ir_proximity();
    el_enable_camera();

    el_launch_process(Routine_DebugControl,NULL);
    el_launch_process(Routine_LED,NULL);

    el_enable_accelerometer();
    trg = el_create_trigger();
    el_trigger_set_process(trg,Trigger_Accelerometer_Process);
    el_trigger_set_event(trg,EL_EVENT_ACCELEROMETER_UPDATE);

    el_main_loop();
    
    return 0;
}

//==============================================================================

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
