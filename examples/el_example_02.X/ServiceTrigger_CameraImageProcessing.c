
#include "ServiceTrigger_CameraImageProcessing.h"

el_handle ST_ImgProc;
volatile int IMG_RedMass;
volatile int IMG_RedBias;
volatile int IMG_GreenMass;
volatile int IMG_GreenBias;
volatile int IMG_BlueMass;
volatile int IMG_BlueBias;

//==============================================================================

bool IsColorRed(unsigned char*RGB){
    unsigned int CR = RGB[0]*13;
    unsigned int CG = RGB[1]*19;
    unsigned int CB = RGB[2]*17;
    return (RGB[0]>=108)&&(CR>=CG)&&(CR>=CB);
}

bool IsColorGreen(unsigned char*RGB){
    unsigned int CR = RGB[0]*16;
    unsigned int CG = RGB[1]*12;
    unsigned int CB = RGB[2]*14;
    return (RGB[0]<160)&&(RGB[1]>=112)&&(RGB[2]<200)&&(CG>=CR)&&(CG>=CB);
}

bool IsColorBlue(unsigned char*RGB){
    int DR = (int)RGB[2] - RGB[0];
    int DG = (int)RGB[2] - RGB[1];
    return (RGB[0]<160)&&(RGB[1]<200)&&(RGB[2]>=104)&&(DR>=24)&&(DG>24);
}


void Trigger_CameraImageProcessing_Process(el_handle this_trigger){
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

    IMG_RedMass = s0;
    if(s0){
        x0 = 20*s0 - x0;
        IMG_RedBias = 5*x0/s0;//gives a value range from -100 to +100
    }else{
        IMG_RedBias = 0;
    }

    IMG_GreenMass = s1;
    if(s1){
        x1 = 20*s1 - x1;
        IMG_GreenBias = 5*x1/s1;//gives a value range from -100 to +100
    }else{
        IMG_GreenBias = 0;
    }
    
    IMG_BlueMass = s2;
    if(s2){
        x2 = 20*s2 - x2;
        IMG_BlueBias = 5*x2/s2;//gives a value range from -100 to +100
    }else{
        IMG_BlueBias = 0;
    }
    
    el_trigger_enable(this_trigger);

}

void Trigger_CameraImageProcessing_Setup(){

    IMG_RedMass = 0;
    IMG_RedBias = 0;
    IMG_GreenMass = 0;
    IMG_GreenBias = 0;
    IMG_BlueMass = 0;
    IMG_BlueBias = 0;
    
    ST_ImgProc = el_create_trigger();
    el_trigger_set_event(ST_ImgProc,EL_EVENT_CAMERA_FRAME_UPDATE);
    el_trigger_set_condition(ST_ImgProc,NULL);
    el_trigger_set_process(ST_ImgProc,Trigger_CameraImageProcessing_Process);
    
}

void Trigger_CameraImageProcessing_Clear(){

    el_delete_trigger(ST_ImgProc);
    ST_ImgProc = NULL;

}
