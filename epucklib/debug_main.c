
#include <stdio.h>
#include "el.h"
#include "elu.h"

EL_PROCESS RoutineX(void*data){

    el_enable_stepper_motor();

    el_set_stepper_motor(0,500);
    el_set_stepper_motor(1,500);

    el_process_wait(1000);

    el_set_stepper_motor(0,0);
    el_set_stepper_motor(1,0);

    el_process_wait(1000);

    el_set_stepper_motor(0,-500);
    el_set_stepper_motor(1,-500);

    el_process_wait(1000);

    el_set_stepper_motor(0,0);
    el_set_stepper_motor(1,0);

    el_process_wait(100);
    
    el_disable_stepper_motor();
    
}

EL_PROCESS RoutineA(void*data){
    while(1){
        
        el_set_led(EL_BODY_LED,EL_ON);
        el_process_wait(500);

        el_set_led(EL_BODY_LED,EL_OFF);
        el_process_wait(500);

    }

}

EL_PROCESS RoutineB(void*data){
    while(1){

        el_set_led(EL_RING_LED_0,EL_ON);
        el_process_wait(100);
        
        el_set_led(EL_RING_LED_0,EL_OFF);
        el_process_wait(100);

    }
}

EL_PROCESS RoutineC(void*data){
    int i,j,k;
    int x,y,f;
    el_camera_image *p;
    
    k = 0;
    while(1){

        f = el_camera_get_frame_counter();
        el_process_wait(1000);
        f = el_camera_get_frame_counter() - f;

        //elu_printf("%d\n",el_camera_get_frame_counter());

        elu_printf("<p>\n");el_process_cooperate();
        
        elu_printf("FPS: %d\n",f);el_process_cooperate();
        elu_printf("FMT: %d\n",565);el_process_cooperate();

        el_camera_lock_frame();
        p = el_camera_get_frame();

        elu_printf("RAW: %d,%d\n",p->dim_x,p->dim_y);el_process_cooperate();
        elu_printf("DAT: %d,%d\n",EL_FRAME_BUFFER_WIDTH,EL_FRAME_BUFFER_HEIGHT);el_process_cooperate();
        
        for(i=0;i<EL_FRAME_BUFFER_HEIGHT;i++){
            for(j=0;j<EL_FRAME_BUFFER_WIDTH;j++){
                elu_printf("%4.4X,",p->RawData[i][j]);el_process_cooperate();
            }
            elu_printf("\n");el_process_cooperate();
        }
        
        el_camera_unlock_frame();
        
        elu_printf("</p>\n");el_process_cooperate();

    }
    
}


void booting_procedure01_selector_barrier(){
    int i;

    // do nothing until selector >= 2
    i = 0;
    while(el_get_selector_value()<2){
        el_set_led(i,1);
        el_polled_wait(100);
        el_set_led(i,0);
        i++;
        i = i%8;
    }
    el_set_led(EL_RING_LED_ALL,0);

}

void booting_procedure02_led_pattern(){
    int i;

    for(i=0;i<8;i++){
        el_set_led(i,1);
        el_polled_wait(20);
    }
    el_polled_wait(200);
    el_set_led(EL_RING_LED_ALL,0);
    
}

void booting_procedure03_say_hello(){
    elu_printf("Hello World!\n");
}


int main(int argc,char*argv[]){
    int i,j;
    void *p0,*p1;

    p0 = malloc(16);

    el_initialization();

    booting_procedure01_selector_barrier();// very essential
    booting_procedure02_led_pattern();// recommended
    booting_procedure03_say_hello();// recommended
    
    p1 = malloc(16);
    elu_printf("%d,%d\n",p0,p1);
    elu_printf("%d,%d\n",&i,&j);

    el_enable_camera();
    
    el_launch_process(RoutineA,NULL);
    el_launch_process(RoutineB,NULL);
    el_launch_process(RoutineC,NULL);
    //el_launch_process(RoutineX,NULL);

    el_main_loop();
    
    return 0;
}
