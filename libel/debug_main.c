
#include <stdio.h>
#include "el.h"
#include "elu.h"

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
    int i,j;
    el_camera_image *p;

    el_led_set(EL_LED_RING_0,1);

    el_camera_lock_frame();
    p = el_camera_get_frame();

    elu_printf("p: %p\n",p);
    elu_printf("<p>\n");
    elu_printf("FPS: %d\n",1);
    elu_printf("FMT: %d\n",565);
    elu_printf("RAW: %d,%d\n",p->width,p->height);
    elu_printf("DAT: %d,%d\n",EL_CAMERA_FRAME_BUFFER_WIDTH,EL_CAMERA_FRAME_BUFFER_HEIGHT);
    for(i=0;i<EL_CAMERA_FRAME_BUFFER_HEIGHT;i++){
        for(j=0;j<EL_CAMERA_FRAME_BUFFER_WIDTH;j++){
            elu_printf("%4.4X,",p->data[i][j]);
        }
        elu_printf("\n");
    }
    el_camera_unlock_frame();
    elu_printf("</p>\n");
    
    el_led_set(EL_LED_RING_0,0);

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
    char c;
    int i;
    int value[8];

    while(1){

        if(el_uart_get_char_counter(EL_UART_1) > 0){

            c = el_uart_get_char(EL_UART_1);
            switch(c){

            case 'f':
                el_led_set(EL_LED_FRONT,EL_TOGGLE);
                break;
                
            case 'p':
                el_launch_process(Routine_TransmitCameraImage,NULL);
                break;

            case 'r':
                elu_printf("%d:",el_ir_proximity_get_counter());
                el_ir_proximity_get_all(EL_IR_REFLECTION,value);
                for(i=0;i<8;i++){
                    elu_printf("\t%d",value[i]);
                }
                elu_printf("\r\n");
                break;
            }
        }

        el_process_cooperate();
    }
}

int main(int argc,char*argv[]){
    int i,j;
    void *p0,*p1;
    el_handle trg;

    p0 = malloc(16);

    el_initialization();

    booting_procedure01_selector_barrier();// very essential
    booting_procedure02_led_pattern();// recommended
    booting_procedure03_say_hello();// recommended

#if 0
    p1 = malloc(16);
    elu_printf("%d,%d\n",p0,p1);
    elu_printf("%d,%d\n",&i,&j);
#endif

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

void booting_procedure01_selector_barrier(){
    int i;

    // do nothing until selector >= 8
    i = 0;
    while(el_get_selector_value()<8){
        el_led_set(i,1);
        el_sleep(125);
        el_led_set(i,0);
        i++;
        i = i%8;
    }
    el_led_set(EL_LED_RING_ALL,0);

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
