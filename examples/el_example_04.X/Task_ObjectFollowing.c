
#include "Task_ObjectFollowing.h"

el_uint8 Task_ObjectColor;


int vision_mass_red;
int vision_bias_red;
int vision_mass_green;
int vision_bias_green;
int vision_mass_blue;
int vision_bias_blue;

el_handle trg_object_following;
el_handle trg_image_processing;

void Trigger_ObjectFollowing_Process(el_handle this_trigger);
void Trigger_ImageProcessing_Process(el_handle this_trigger);

//------------------------------------------------------------------------------

void Task_ObjectFollowing_Setup(){
    
    Task_ObjectColor = TASK_OBJECT_GREEN;
    
    vision_mass_red = 0;
    vision_bias_red = 0;
    vision_mass_green = 0;
    vision_bias_green = 0;
    vision_mass_blue = 0;
    vision_bias_blue = 0;
    
    // create a trigger to execute the motion controller based on proximity sensors
    trg_object_following = el_create_trigger();
    el_trigger_set_event(trg_object_following,EL_EVENT_IR_PROXIMITY_UPDATE);
    el_trigger_set_condition(trg_object_following,NULL);
    el_trigger_set_process(trg_object_following,Trigger_ObjectFollowing_Process);

    // create a trigger to execute an image processing algorithm to provide vision information
    trg_image_processing = el_create_trigger();
    el_trigger_set_event(trg_image_processing,EL_EVENT_CAMERA_FRAME_UPDATE);
    el_trigger_set_condition(trg_image_processing,NULL);
    el_trigger_set_process(trg_image_processing,Trigger_ImageProcessing_Process);

    // configure involved components
    el_stepper_motor_options_reset();
    el_stepper_motor_options()->UseAcceleration = true;
    el_stepper_motor_options()->AccelerationRate = 2000;
    el_stepper_motor_options_apply();
    
    el_camera_options()->ExposureMode = EL_AUTOMATIC;
    el_camera_options()->AutoWhiteBalance = true;
    el_camera_options()->AutoDigitalGain = true;
    el_camera_options_apply();// the above setting are same as default

    // enable involved components
    el_enable_camera();
    el_enable_ir_proximity();
    el_enable_stepper_motor();
    
    el_led_set(EL_LED_BODY,EL_ON);
}

void Task_ObjectFollowing_Clear(){

    el_led_set(EL_LED_BODY,EL_OFF);

    el_disable_camera();
    el_disable_ir_proximity();
    el_disable_stepper_motor();

    el_delete_trigger(trg_image_processing);
    trg_image_processing = NULL;

    el_delete_trigger(trg_object_following);
    trg_object_following = NULL;

}

//------------------------------------------------------------------------------

void Trigger_ObjectFollowing_Process(el_handle this_trigger){
    int w,x;
    int ir0,ir7;
    int LinearSpeed,AngularSpeed;
    int SpeedLeft,SpeedRight;
    
    switch(Task_ObjectColor){
        
    case TASK_OBJECT_RED:
        w = vision_mass_red;
        x = vision_bias_red;
        break;

    case TASK_OBJECT_GREEN:
        w = vision_mass_green;
        x = vision_bias_green;
        break;

    case TASK_OBJECT_BLUE:
        w = vision_mass_blue;
        x = vision_bias_blue;
        break;

    default:
        w = 0;
        x = 0;
        break;
    }

    // the the readings of the two front proximity sensors
    el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_0,EL_IR_REFLECTION,&ir0);
    el_ir_proximity_get(EL_IR_PROXIMITY_SENSOR_7,EL_IR_REFLECTION,&ir7);
    
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
        el_led_set(EL_LED_RING_4,EL_ON);
    }else{
        el_led_set(EL_LED_RING_4,EL_OFF);
    }
    
    SpeedLeft = LinearSpeed - AngularSpeed;
    SpeedRight = LinearSpeed + AngularSpeed;

    // apply the speed values to the motors
    el_set_wheel_speed(SpeedLeft,SpeedRight);
    
    el_trigger_enable(this_trigger);
}

//------------------------------------------------------------------------------

el_bool IsColorRed(const el_uint8*RGB){
    unsigned int CR = RGB[0]*13;
    unsigned int CG = RGB[1]*19;
    unsigned int CB = RGB[2]*17;
    return (RGB[0]>=104)&&(CR>=CG)&&(CR>=CB);
}

el_bool IsColorGreen(const el_uint8*RGB){
    unsigned int CR = RGB[0]*16;
    unsigned int CG = RGB[1]*12;
    unsigned int CB = RGB[2]*14;
    return (RGB[1]>=92)&&(RGB[0]<160)&&(RGB[2]<200)&&(CG>=CR)&&(CG>=CB);
}

el_bool IsColorBlue(const el_uint8*RGB){
    int DR = (int)RGB[2] - RGB[0];
    int DG = (int)RGB[2] - RGB[1];
    return (RGB[2]>=96)&&(RGB[0]<160)&&(RGB[1]<200)&&(DR>=20)&&(DG>20);
}

void Trigger_ImageProcessing_Process(el_handle this_trigger){
    el_uint8 RGB[3];
    int X,Y;
    int x0,s0,x1,s1,x2,s2;

    s0 = 0;
    x0 = 0;
    s1 = 0;
    x1 = 0;
    s2 = 0;
    x2 = 0;

    el_camera_lock_frame();
    for(X=0;X<EL_CAMERA_FRAME_DIM_X;X++){
        for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){

            el_camera_get_frame_pixel(X,Y,RGB);

            // center of mass algorithm

            if(IsColorRed(RGB)){
                s0++;// total mass
                x0 += X*1;// offset*mass
            }

            if(IsColorGreen(RGB)){
                s1++;
                x1 += X*1;
            }

            if(IsColorBlue(RGB)){
                s2++;
                x2 += X*1;
            }

        }
    }
    el_camera_unlock_frame();

    vision_mass_red = s0;
    if(s0){
        x0 = x0 - 20*s0;
        vision_bias_red = 5*x0/s0;//gives a value range from -100 to +100
    }else{
        vision_bias_red = 0;
    }

    vision_mass_green = s1;
    if(s1){
        x1 = x1 - 20*s1;
        vision_bias_green = 5*x1/s1;//gives a value range from -100 to +100
    }else{
        vision_bias_green = 0;
    }

    vision_mass_blue = s2;
    if(s2){
        x2 = x2 - 20*s2;
        vision_bias_blue = 5*x2/s2;//gives a value range from -100 to +100
    }else{
        vision_bias_blue = 0;
    }
    
    el_trigger_enable(this_trigger);
}
