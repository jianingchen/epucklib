
#include "TaskTrigger_ObjectFollowing.h"

el_handle TT_ObjectFollowing;
el_uint8 TT_ObjectColor;



void Trigger_ObjectFollowing_Process(el_handle this_trigger){
    int w,x;
    int ir0,ir7;
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
    
    ir0 = el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_0,EL_IR_REFLECTION);
    ir7 = el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_7,EL_IR_REFLECTION);
    
    // calculate the angular speed according to the horizontal bias of the color
    if(w >= 8){
        LinearSpeed = 600;
        AngularSpeed = -4*x;
        el_led_set(EL_LED_RING_0,EL_ON);
    }else{
        LinearSpeed = 0;
        AngularSpeed = 0;
        el_led_set(EL_LED_RING_0,EL_OFF);
    }

    // do not chase if any object is closely in front of the robot
    if((ir0 + ir7) > 120){
        LinearSpeed = 0;
    }
    
    SpeedLeft = LinearSpeed - AngularSpeed;
    SpeedRight = LinearSpeed + AngularSpeed;

    // apply the speed values to the motors
    el_set_wheel_speed(SpeedLeft,SpeedRight);
    
    el_trigger_enable(this_trigger);
}

void Trigger_ObjectFollowing_Setup(){

    TT_ObjectColor = TT_OBJECT_COLOR_GREEN;

    TT_ObjectFollowing = el_create_trigger();
    el_trigger_set_event(TT_ObjectFollowing,EL_EVENT_IR_PROXIMITY_UPDATE);
    el_trigger_set_condition(TT_ObjectFollowing,NULL);
    el_trigger_set_process(TT_ObjectFollowing,Trigger_ObjectFollowing_Process);
    
}

void Trigger_ObjectFollowing_Clear(){

    el_delete_trigger(TT_ObjectFollowing);
    TT_ObjectFollowing = NULL;

}

