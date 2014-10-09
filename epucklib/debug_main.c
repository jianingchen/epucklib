
#include <stdio.h>
#include "el.h"
#include "elu.h"

void booting_procedure01_selector_barrier();
void booting_procedure02_led_pattern();
void booting_procedure03_say_hello();



EL_PROCESS RoutineA(void*data){
    while(1){

        el_led_set(EL_LED_BODY,1);
        el_process_wait(500);

        el_led_set(EL_LED_BODY,0);
        el_process_wait(500);

    }

}

EL_PROCESS RoutineB(void*data){
    while(1){

        el_led_set(EL_LED_RING_0,1);
        el_process_wait(100);

        el_led_set(EL_LED_RING_0,0);
        el_process_wait(100);

    }
}

EL_PROCESS RoutineMotorTest(void*data){

    el_enable_stepper_motor();

    el_stepper_motor_set(EL_STEPPER_MOTOR_LEFT,500);
    el_stepper_motor_set(EL_STEPPER_MOTOR_RIGHT,500);

    el_process_wait(1000);

    el_stepper_motor_set(EL_STEPPER_MOTOR_LEFT,0);
    el_stepper_motor_set(EL_STEPPER_MOTOR_RIGHT,0);

    el_process_wait(1000);

    el_stepper_motor_set(EL_STEPPER_MOTOR_LEFT,-500);
    el_stepper_motor_set(EL_STEPPER_MOTOR_RIGHT,-500);

    el_process_wait(1000);

    el_stepper_motor_set(EL_STEPPER_MOTOR_LEFT,0);
    el_stepper_motor_set(EL_STEPPER_MOTOR_RIGHT,0);

    el_process_wait(100);
    
    el_disable_stepper_motor();
    
}

EL_PROCESS Routine_CameraDebugLoop(void*data){
    int i,j,k;
    int x,y,f;
    el_camera_image *p;

    el_enable_camera();

    k = 0;
    while(1){

        f = el_camera_get_frame_counter();
        el_process_wait(1000);
        f = el_camera_get_frame_counter() - f;

        //elu_printf("%d\n",el_camera_get_frame_counter());

        elu_printf("<p>\n");el_process_cooperate();
        
        elu_printf("FPS: %d\n",f);
        el_process_cooperate();
        elu_printf("FMT: %d\n",565);
        el_process_cooperate();

        el_camera_lock_frame();
        p = el_camera_get_frame();

        elu_printf("RAW: %d,%d\n",p->dim_x,p->dim_y);
        el_process_cooperate();
        elu_printf("DAT: %d,%d\n",EL_CAMERA_FRAME_BUFFER_WIDTH,EL_CAMERA_FRAME_BUFFER_HEIGHT);
        el_process_cooperate();
        
        for(i=0;i<EL_CAMERA_FRAME_BUFFER_HEIGHT;i++){
            for(j=0;j<EL_CAMERA_FRAME_BUFFER_WIDTH;j++){
                elu_printf("%4.4X,",p->RawData[i][j]);
                el_process_cooperate();
            }
            elu_printf("\n");
            el_process_cooperate();
        }
        
        el_camera_unlock_frame();
        
        elu_printf("</p>\n");
        el_process_cooperate();

    }
    
}

EL_PROCESS Routine_ProximityDebugLoop(void*data){
    int value[8];
    int i;
    
    el_config_ir_proximity(EL_IR_PROXIMITY_MODE_PULSE);
    
    el_enable_ir_proximity();
    
    while(1){

        el_process_wait(1000);

        el_ir_proximity_get_all(EL_IR_PROXIMITY_GET_REFLECTION,value);
        elu_printf("%d: ",el_ir_proximity_get_counter());
        el_process_cooperate();
        for(i=0;i<8;i++){
            elu_printf("%d,",value[i]);
            el_process_cooperate();
        }
        elu_printf("\n");
        el_process_cooperate();

    }
    
    el_disable_ir_proximity();
}

EL_CONDITION ProximitySensors_Condition(el_handle this_trigger){
    if(el_ir_proximity_get_counter()%30){
        return false;
    }
    return true;
}

EL_PROCESS ProximitySensors_Process(el_handle this_trigger){
    int i;
    
    i = el_trigger_get_counter(this_trigger)%8;

    el_led_set(i,1);
    el_process_wait(200);
    el_led_set(i,0);
}

EL_PROCESS AccelerometerFeedback_Process(el_handle this_trigger){
    int v;

    v = el_accelerometer_get(EL_ACCELEROMETER_GET_X);
    elu_printf("%d,",v);
    el_process_cooperate();
    
    v = el_accelerometer_get(EL_ACCELEROMETER_GET_Y);
    elu_printf("%d,",v);
    el_process_cooperate();

    v = el_accelerometer_get(EL_ACCELEROMETER_GET_Z);
    elu_printf("%d,",v);
    el_process_cooperate();

    elu_printf("\n");

}

EL_PROCESS Accelerometer_Process(el_handle this_trigger){
    static int previous_xyz[3] = {0,0,0};
    const long threshold_s = 250L*250L;
    long magnitude_s;// _s means squared;
    int xyz[3];
    int dx,dy,dz;

    el_trigger_disable(this_trigger);

    el_accelerometer_get_all(xyz);
    if(previous_xyz[0]!=0){
        dx = xyz[0] - previous_xyz[0];
        dy = xyz[1] - previous_xyz[1];
        dz = xyz[2] - previous_xyz[2];
        magnitude_s = (long)dx*dx;
        magnitude_s += (long)dy*dy;
        magnitude_s += (long)dz*dz;
        if(magnitude_s > threshold_s){
            el_led_set(EL_LED_RING_4,EL_LED_ON);
            el_process_wait(200);
            el_led_set(EL_LED_RING_4,EL_LED_OFF);
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

#if 0
    el_launch_process(Routine_ProximityDebugLoop,NULL);
    trg = el_create_trigger();
    el_trigger_register_condition(trg,ProximitySensors_Condition);
    el_trigger_register_process(trg,ProximitySensors_Process);
    el_trigger_register_exclusive_event(trg,EL_EVENT_IR_PROXIMITY_UPDATE);
#endif

#if 1
    el_enable_accelerometer();

    trg = el_create_trigger();
    el_trigger_register_process(trg,AccelerometerFeedback_Process);
    el_trigger_register_timing_event(trg,200,true);

    trg = el_create_trigger();
    el_trigger_register_process(trg,Accelerometer_Process);
    el_trigger_register_exclusive_event(trg,EL_EVENT_ACCELEROMETER_UPDATE);
    
#endif

#if 0
    el_launch_process(Routine_CameraDebugLoop,NULL);
#endif


    el_main_loop();
    
    return 0;
}

void booting_procedure01_selector_barrier(){
    int i;

    // do nothing until selector >= 2
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
    
    el_led_set(EL_LED_RING_ALL,EL_LED_ON);
    el_sleep(500);
    el_led_set(EL_LED_RING_ALL,EL_LED_OFF);
    el_sleep(500);
    
}

void booting_procedure03_say_hello(){
    elu_printf("Hello World!\n");
}
