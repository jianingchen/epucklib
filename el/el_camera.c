/*

embedded system library for e-puck

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

#include "el_context.h"
#include "el_camera.h"
#include <math.h>

el_camera_image el_frame_buffer_a;
el_camera_image el_frame_buffer_b;
el_uint16 el_cam_device_id;
el_uint16 el_cam_revision_n;
el_uint8 el_cam_auto_function;
bool el_cam_enabled;
volatile el_uint16 el_cam_lock_buffer;
volatile el_uint32 el_cam_frame_counter;
el_camera_image *el_cam_r_frame;//for reading
el_camera_image *el_cam_w_frame;//for writing
el_uint16 *el_cam_line_pointer;
el_uint16 *el_cam_pixel_pointer;
el_uint16 el_cam_x;
el_uint16 el_cam_y;

el_camera_param el_cam_parameters;

//------------------------------------------------------------------------------

static el_uint8 el_cam_register_read_uint8(el_uint8 address){
    return e_i2cp_read(EL_CAM_I2C_ID,address);
}

static void el_cam_register_write_uint8(el_uint8 address,el_uint8 b){
    e_i2cp_write(EL_CAM_I2C_ID,address,b);
}

static el_uint16 el_cam_register_read_uint16(el_uint8 address){
    el_uint8 H;
    el_uint8 L;
    H = e_i2cp_read(EL_CAM_I2C_ID,address);
    L = e_i2cp_read(EL_CAM_I2C_ID,address+1);
    return (H<<8)|L;
}
/*
static void el_cam_register_write_uint16(el_uint8 address,el_uint16 w){
    el_uint8 H = w>>8;
    el_uint8 L = w&0xFF;
    e_i2cp_write(EL_CAM_I2C_ID,address,H);
    e_i2cp_write(EL_CAM_I2C_ID,address+1,L);
}
*/
//------------------------------------------------------------------------------

void el_init_camera(){
    int i;
    int X,Y;
    
    CAM_VSYNC_DIR = INPUT_PIN;
    CAM_HREF_DIR = INPUT_PIN;
    CAM_PCLK_DIR = INPUT_PIN;
    
    el_cam_enabled = 0;
    el_cam_lock_buffer = 0;
    el_cam_frame_counter = 0;
    el_cam_x = 0;
    el_cam_y = 0;
    el_cam_r_frame = &el_frame_buffer_a;
    el_cam_w_frame = &el_frame_buffer_b;
    el_cam_line_pointer = &el_cam_w_frame->Data[0][0];
    el_cam_pixel_pointer = &el_cam_w_frame->Data[0][0];

    el_cam_device_id = 0;
    el_cam_revision_n = 0;
    el_cam_auto_function = 0;

    // reset the camera module, use the duration required to initialize the buffers

    CAM_RESET = 0;

    el_frame_buffer_a.Width = 0;
    el_frame_buffer_a.Height = 0;
    for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){
        for(X=0;X<EL_CAMERA_FRAME_DIM_X;X++){
            el_frame_buffer_a.Data[Y][X] = 0;
        }
    }
    
    CAM_RESET = 1;

    el_frame_buffer_b.Width = 0;
    el_frame_buffer_b.Height = 0;
    for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){
        for(X=0;X<EL_CAMERA_FRAME_DIM_X;X++){
            el_frame_buffer_b.Data[Y][X] = 0;
        }
    }

    el_camera_options_reset();

    // reset the camera module
    el_cam_init_register();

}

void el_cam_init_register(void){

    e_i2cp_enable();

    // read the camera model info
    el_cam_device_id = el_cam_register_read_uint16(0x00);
    el_cam_revision_n = el_cam_register_read_uint16(0x02);

    // set target to register group B
    el_cam_register_write_uint8(0x03,0x01);

    // change clock rate
    el_cam_register_write_uint8(0x38,0x08);// color format
    el_cam_register_write_uint8(0x68,0x01);// pixel clock rate
    el_cam_register_write_uint8(0x51,0x04);
    el_cam_register_write_uint8(0x53,0x04);
    el_cam_register_write_uint8(0x54,0x01);
    el_cam_register_write_uint8(0x55,0x43);
    el_cam_register_write_uint8(0x56,0x00);
    el_cam_register_write_uint8(0x57,0xF3);
    el_cam_register_write_uint8(0x61,0x0C);
    el_cam_register_write_uint8(0x63,0xEC);
    el_cam_register_write_uint8(0x80,0x40);
    el_cam_register_write_uint8(0x81,0x40);
    el_cam_register_write_uint8(0x82,0x01);

    // enable horizontal mirror
    
    el_cam_register_write_uint8(0x68,0b01100001);
    el_cam_register_write_uint8(0x03,0x00);// switch to group A
    el_cam_register_write_uint8(0x90,0b01110101);
    el_nop_delay(50);
    el_cam_register_write_uint8(0x03,0x01);// switch to group B
    el_cam_register_write_uint8(0x68,0b00000001);

    // use test pattern
    //el_cam_register_write_uint8(0x40,0x1A);

    // set target to register group C
    el_cam_register_write_uint8(0x03,0x02);
    el_cam_auto_function = el_cam_register_read_uint8(0x04);


    // change image format to 320x240
    el_cam_register_write_uint8(0x11,0x12);
    el_cam_register_write_uint8(0x13,0x0E);
    el_cam_register_write_uint8(0x14,0x01);
    el_cam_register_write_uint8(0x15,0x30);
    el_cam_register_write_uint8(0x16,0x00);
    el_cam_register_write_uint8(0x17,0xDF);
    el_cam_register_write_uint8(0x19,0x72);
    el_cam_register_write_uint8(0x1B,0x43);
    el_cam_register_write_uint8(0x1D,0x50);
    el_cam_register_write_uint8(0x1F,0x50);

    // set exposure control & white balance
    el_cam_register_write_uint8(0x04,0b10011000);// Auto White Balance ON + AUTO Exposure Control
    el_cam_register_write_uint8(0x05,0b00001001);// Auto Digital Gain
    //el_cam_register_write_uint8(0x24,0b00000000);// External Integration Time H
    //el_cam_register_write_uint8(0x25,0b10000000);// External Integration Time M
    //el_cam_register_write_uint8(0x26,0b00000000);// External Integration Time L
    //el_cam_register_write_uint8(0x28,0x01);// External Linear Gain
    //el_cam_register_write_uint8(0x29,0x00);// External Linear Gain
    //el_cam_register_write_uint8(0x2C,0x00);// Exposure Time T
    //el_cam_register_write_uint8(0x2D,0x00);// Exposure Time H
    //el_cam_register_write_uint8(0x2E,0x80);// Exposure Time M
    //el_cam_register_write_uint8(0x2F,0x00);// Exposure Time L
    //el_cam_register_write_uint8(0x4C,0b00000000);// Integration Time H
    //el_cam_register_write_uint8(0x4D,0b10000000);// Integration Time M
    //el_cam_register_write_uint8(0x4E,0b00000000);// Integration Time L
    //el_cam_register_write_uint8(0x50,0x00);// Global Gain
    //el_cam_register_write_uint8(0x51,64*1.0000);// Digital Gain
    //el_cam_register_write_uint8(0xA4,64*1.0000);//Red Gain
    //el_cam_register_write_uint8(0xA5,64*1.0000);//Green Gain
    //el_cam_register_write_uint8(0xA6,64*1.0000);//Blue Gain

#ifdef EL_CAM_USE_STANDBY
    // set target to register group B
    el_cam_register_write_uint8(0x03,0x01);
    el_cam_register_write_uint8(0x34,0x02);// standby bit
#endif
    
    // set target to register group A
    el_cam_register_write_uint8(0x03,0x00);
    

    e_i2cp_disable();

}

el_camera_param*el_camera_options(){
    return &el_cam_parameters;
}

void el_camera_options_reset(){
    el_cam_parameters.ExposureMode = EL_AUTOMATIC;
    el_cam_parameters.AutoWhiteBalance = true;
    el_cam_parameters.AutoDigitalGain = true;
    el_cam_parameters.ExposureTime = 1.0f;
    el_cam_parameters.RedGain = 1.0f;
    el_cam_parameters.GreenGain = 1.0f;
    el_cam_parameters.BlueGain = 1.0f;
    el_cam_parameters.ExternalIntergationTime = 1.0f;
    el_cam_parameters.ExternalLinearGain = 1.0f;
    el_cam_parameters.IntegrationTime = 1.0f;
    el_cam_parameters.GlobalGain = 1.0f;
    el_cam_parameters.DigitalGain = 1.0f;
}

void el_camera_options_apply(){
    el_config_camera(&el_cam_parameters);
}

void el_config_camera(const el_camera_param*p){
    union{
        uint32_t dword;
        struct{
            uint8_t byteL;
            uint8_t byteM;
            uint8_t byteH;
            uint8_t byteT;
        };
    } FixedPoint;

    union{
        uint8_t byte;
        struct{
            uint8_t ExposureMode:2;
            uint8_t AutoWhiteBalanceOFF:1;
        };
    } AutoControl_1;
    
    union{
        uint8_t byte;
        struct{
            uint8_t Reserved:2;
            uint8_t AutoDigitalGain:1;
        };
    } AutoControl_2;
    
    AutoControl_1.byte = 0b10011000;
    AutoControl_2.byte = 0b00001101;
    
    AutoControl_1.ExposureMode = p->ExposureMode;
    AutoControl_1.AutoWhiteBalanceOFF = !p->AutoWhiteBalance;
    AutoControl_2.AutoDigitalGain = p->AutoDigitalGain;

    e_i2cp_enable();

    el_cam_register_write_uint8(0x03,0x02);// Register Group C
    
    el_cam_register_write_uint8(0x04,AutoControl_1.byte);
    el_cam_register_write_uint8(0x05,AutoControl_2.byte);
    
    FixedPoint.dword = 32768*p->ExternalIntergationTime;
    el_cam_register_write_uint8(0x24,FixedPoint.byteH);// External Integration Time H
    el_cam_register_write_uint8(0x25,FixedPoint.byteM);// External Integration Time M
    el_cam_register_write_uint8(0x26,FixedPoint.byteL);// External Integration Time L

    FixedPoint.dword = 256*p->ExternalLinearGain;
    el_cam_register_write_uint8(0x28,FixedPoint.byteM);// External Linear Gain H
    el_cam_register_write_uint8(0x29,FixedPoint.byteL);// External Linear Gain L
    
    FixedPoint.dword = 32768*p->ExposureTime;
    el_cam_register_write_uint8(0x2C,FixedPoint.byteT);// Exposure Time T
    el_cam_register_write_uint8(0x2D,FixedPoint.byteH);// Exposure Time H
    el_cam_register_write_uint8(0x2E,FixedPoint.byteM);// Exposure Time M
    el_cam_register_write_uint8(0x2F,FixedPoint.byteL);// Exposure Time L

    FixedPoint.dword = 32768*p->IntegrationTime;
    el_cam_register_write_uint8(0x4C,FixedPoint.byteH);// Integration Time H
    el_cam_register_write_uint8(0x4D,FixedPoint.byteM);// Integration Time M
    el_cam_register_write_uint8(0x4E,FixedPoint.byteL);// Integration Time L

    FixedPoint.dword = (int)(15.8f*log(p->GlobalGain)/log(2.0f));
    el_cam_register_write_uint8(0x50,FixedPoint.byteL);// Global Gain
    
    el_cam_register_write_uint8(0x51,64*p->DigitalGain);
    el_cam_register_write_uint8(0xA4,64*p->RedGain);
    el_cam_register_write_uint8(0xA5,64*p->GreenGain);
    el_cam_register_write_uint8(0xA6,64*p->BlueGain);

    el_cam_register_write_uint8(0x03,0x00);// Register Group A

    e_i2cp_disable();
    
}



void el_enable_camera(){

    if(!el_cam_enabled){

#ifdef EL_CAM_USE_STANDBY
        e_i2cp_enable();

        // set target to register group B
        el_cam_register_write_uint8(0x03,0x01);
        // set standby bit to 0
        el_cam_register_write_uint8(0x34,0x00);
        // set target to register group A
        el_cam_register_write_uint8(0x03,0x00);

        e_i2cp_disable();
#endif

        IEC0bits.T1IE = 1;// enable pixel interrupt (T1 Interrupt)
        IEC1bits.T4IE = 1;// enable line interrupt (T4 Interrupt)
        IEC1bits.T5IE = 1;// enable frame interrupt (T5 Interrupt)
        T1CONbits.TON = 0;
        T4CONbits.TON = 0;
        T5CONbits.TON = 1;

        el_cam_enabled = 1;
    }

}

void el_disable_camera(){
    if(el_cam_enabled){

        IEC0bits.T1IE = 0;
        IEC1bits.T4IE = 0;
        IEC1bits.T5IE = 0;
        T1CONbits.TON = 0;
        T4CONbits.TON = 0;
        T5CONbits.TON = 0;

#ifdef EL_CAM_USE_STANDBY
        e_i2cp_enable();

        // set target to register group B
        el_cam_register_write_uint8(0x03,0x01);
        // set standby bit to 1
        el_cam_register_write_uint8(0x34,0x02);
        // set target to register group A
        el_cam_register_write_uint8(0x03,0x00);

        e_i2cp_disable();
#endif

        el_cam_x = 0;
        el_cam_y = 0;
        el_cam_line_pointer = (el_uint16*)el_cam_w_frame->Data;
        el_cam_pixel_pointer = (el_uint16*)el_cam_w_frame->Data;

        el_cam_enabled = 0;
    }
}

void el_cam_swap_buffer(){
    el_camera_image* temp;

    temp = el_cam_w_frame;
    el_cam_w_frame = el_cam_r_frame;
    el_cam_r_frame = temp;

}

void el_camera_lock_frame(){
    el_cam_lock_buffer = 1;
}

void el_camera_unlock_frame(){
    el_cam_lock_buffer = 0;
}

bool el_camera_is_frame_locked(){
    return el_cam_lock_buffer;
}

el_uint32 el_camera_get_frame_counter(){
    return el_cam_frame_counter;
}

el_camera_image*el_camera_frame(){
    return el_cam_r_frame;
}

void el_camera_get_frame_pixel(int X,int Y,el_uint8*rgb3v){
    const el_uint16 red_bits   = 0b1111100000000000;
    const el_uint16 green_bits = 0b0000011111100000;
    const el_uint16 blue_bits  = 0b0000000000011111;
    el_uint16 w = el_cam_r_frame->Data[Y][X];
    rgb3v[0] = (w&red_bits)>>8;
    rgb3v[1] = (w&green_bits)>>3;
    rgb3v[2] = (w&blue_bits)<<3;
}

unsigned int el_camera_get_frame_width(){
    return el_cam_r_frame->Width;
}

unsigned int el_camera_get_frame_height(){
    return el_cam_r_frame->Height;
}
