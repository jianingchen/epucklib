
#include "TaskTrigger_ObjectFollowing.h"

el_handle TT_ObjectFollowing;
el_uint8 TT_ObjectColor;



void Trigger_ObjectFollowing_Process(el_handle this_trigger){
    int w,x;
    int ir_reflection[8];
    int LinearSpeed,AngularSpeed;
    int SpeedLeft,SpeedRight;

    switch(TT_ObjectColor){
        
    case TT_OBJECT_COLOR_RED:
        w = IMG_RedMass;
        x = IMG_RedBias;
        break;

    case TT_OBJECT_COLOR_GREEN:
        w = IMG_GreenMass;
        x = IMG_GreenBias;
        break;

    case TT_OBJECT_COLOR_BLUE:
        w = IMG_BlueMass;
        x = IMG_BlueBias;
        break;

    default:
        w = 0;
        x = 0;
        break;
    }
    
    el_ir_proximity_get_all(EL_IR_REFLECTION,ir_reflection);

    // calculate the angular speed according to the horizontal bias of the color
    if(w >= 8){
        LinearSpeed = 500;
        AngularSpeed = -4*x;
        el_led_set(EL_LED_RING_0,EL_ON);
    }else{
        LinearSpeed = 0;
        AngularSpeed = 0;
        el_led_set(EL_LED_RING_0,EL_OFF);
    }

    // do not chase if any object is closely in front of the robot
    if((ir_reflection[0] + ir_reflection[7]) > 120 ){
        LinearSpeed = 0;
    }
    
    SpeedLeft = LinearSpeed - AngularSpeed;
    SpeedRight = LinearSpeed + AngularSpeed;

    // apply the speed values to the motor
    el_stepper_motor_set_speed(EL_STEPPER_MOTOR_LEFT,SpeedLeft);
    el_stepper_motor_set_speed(EL_STEPPER_MOTOR_RIGHT,SpeedRight);
    
    el_trigger_enable(this_trigger);
}

void Trigger_ObjectFollowing_Setup(){

    TT_ObjectColor = 1;

    TT_ObjectFollowing = el_create_trigger();
    el_trigger_set_event(TT_ObjectFollowing,EL_EVENT_IR_PROXIMITY_UPDATE);
    el_trigger_set_condition(TT_ObjectFollowing,NULL);
    el_trigger_set_process(TT_ObjectFollowing,Trigger_ObjectFollowing_Process);
    
}

void Trigger_ObjectFollowing_Clear(){

    el_delete_trigger(TT_ObjectFollowing);
    TT_ObjectFollowing = NULL;

}

