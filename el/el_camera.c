
#include "el_context.h"
#include "el_camera.h"

el_camera_image el_frame_buffer_a;
el_camera_image el_frame_buffer_b;
uint16_t el_cam_device_id;
uint16_t el_cam_revision_n;
uint8_t el_cam_auto_function;

bool el_cam_enabled;
volatile bool el_cam_lock_buffer;
volatile uint16_t el_cam_frame_counter;
el_camera_image *el_cam_r_frame;//for reading
el_camera_image *el_cam_w_frame;//for writing
uint16_t *el_cam_line_pointer;
uint16_t *el_cam_pixel_pointer;
uint16_t el_cam_x;
uint16_t el_cam_y;

static uint8_t el_cam_register_read_uint8(uint8_t address){
    return e_i2cp_read(EL_CAM_I2C_ID,address);
}

static void el_cam_register_write_uint8(uint8_t address,uint8_t b){
    e_i2cp_write(EL_CAM_I2C_ID,address,b);
}

static uint16_t el_cam_register_read_uint16(uint8_t address){
    uint8_t H;
    uint8_t L;
    H = e_i2cp_read(EL_CAM_I2C_ID,address);
    L = e_i2cp_read(EL_CAM_I2C_ID,address+1);
    return (H<<8)|L;
}

static void el_cam_register_write_uint16(uint8_t address,uint16_t w){
    uint8_t H = w>>8;
    uint8_t L = w&0xFF;
    e_i2cp_write(EL_CAM_I2C_ID,address,H);
    e_i2cp_write(EL_CAM_I2C_ID,address+1,L);
}

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
    el_cam_line_pointer = (uint16_t*)el_cam_w_frame->data;
    el_cam_pixel_pointer = (uint16_t*)el_cam_w_frame->data;

    el_cam_device_id = 0;
    el_cam_revision_n = 0;
    el_cam_auto_function = 0;

    el_frame_buffer_a.width = 0;
    el_frame_buffer_a.height = 0;
    el_frame_buffer_b.width = 0;
    el_frame_buffer_b.height = 0;
    for(Y=0;Y<EL_CAMERA_FRAME_BUFFER_HEIGHT;Y++){
        for(X=0;X<EL_CAMERA_FRAME_BUFFER_WIDTH;X++){
            el_frame_buffer_a.data[Y][X] = 0;
            el_frame_buffer_b.data[Y][X] = 0;
        }
    }
    
    // reset the camera module
    CAM_RESET = 0;
    i = 100;while(i--) NOP();
    CAM_RESET = 1;
    i = 100;while(i--) NOP();
    
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
    el_cam_register_write_uint8(0x04,0b10011010);//Auto White Balance ON + Exposure Control using EIT
    el_cam_register_write_uint8(0x24,0b00000000);//External Integration Time H
    el_cam_register_write_uint8(0x25,0b10000000);//External Integration Time M
    el_cam_register_write_uint8(0x26,0b00000000);//External Integration Time L
    el_cam_register_write_uint16(0x28,0x1000);//External Linear Gain
    el_cam_register_write_uint8(0x55,0x00);
    el_cam_register_write_uint8(0x56,0x00);
    el_cam_register_write_uint8(0xA4,64*1.0000);//Red Gain
    el_cam_register_write_uint8(0xA5,64*1.0000);//Green Gain
    el_cam_register_write_uint8(0xA6,64*1.0000);//Blue Gain
    //e_i2cp_write(EL_CAM_I2C_ID,0xA6,0b01001111);//Blue Gain = (fix point)1.234375

#ifdef EL_CAM_USE_STANDBY
    // set target to register group B
    el_cam_register_write_uint8(0x03,0x01);
    el_cam_register_write_uint8(0x34,0x01);// standby bit
#endif

    // set target to register group A
    el_cam_register_write_uint8(0x03,0x00);

    e_i2cp_disable();

}

void el_config_camera(el_camera_ini*setting){
    uint8_t R = 64*setting->RedGain;
    uint8_t G = 64*setting->GreenGain;
    uint8_t B = 64*setting->BlueGain;
    uint16_t LG = 4096*setting->LinearGain;
    union{
        uint32_t dword;
        struct{
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        };
    } EIT;

    union{
        uint8_t byte;
        struct{
            uint8_t bit0_1_ExposureMode:2;
            uint8_t bit2_FixedWhiteBalance:1;
            uint8_t bit3_unused:1;
            uint8_t bit4_unused:1;
            uint8_t bit5_unused:1;
            uint8_t bit6_unused:1;
            uint8_t bit7_unused:1;
        };
    } flags;

    EIT.dword = 32768*setting->ExternalIntergationTime;

    flags.byte = 0;
    flags.bit0_1_ExposureMode = setting->ExposureMode;
    flags.bit2_FixedWhiteBalance = !setting->AutoWhiteBalance;


    e_i2cp_enable();

    // set target to register group C
    el_cam_register_write_uint8(0x03,0x02);

    el_cam_register_write_uint8(0x04,flags.byte);
    el_cam_register_write_uint8(0x24,EIT.byte2);//External Integration Time H
    el_cam_register_write_uint8(0x25,EIT.byte1);//External Integration Time M
    el_cam_register_write_uint8(0x26,EIT.byte0);//External Integration Time L
    el_cam_register_write_uint16(0x28,LG);//External Linear Gain
    el_cam_register_write_uint8(0x55,0x00);
    el_cam_register_write_uint8(0x56,0x00);
    el_cam_register_write_uint8(0xA4,R);//Red Gain
    el_cam_register_write_uint8(0xA5,G);//Green Gain
    el_cam_register_write_uint8(0xA6,B);//Blue Gain

    // set target to register group A
    el_cam_register_write_uint8(0x03,0x00);

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
        T5CONbits.TON = 1;

        el_cam_enabled = 1;
    }

}

void el_disable_camera(){
    if(el_cam_enabled){

        IEC0bits.T1IE = 0;// disable pixel interrupt (T1 Interrupt)
        IEC1bits.T4IE = 0;// disable line interrupt (T4 Interrupt)
        IEC1bits.T5IE = 0;// disable frame interrupt (T5 Interrupt)
        T5CONbits.TON = 0;
        T4CONbits.TON = 0;
        T1CONbits.TON = 0;

#ifdef EL_CAM_USE_STANDBY
        e_i2cp_enable();

        // set target to register group B
        el_cam_register_write_uint8(0x03,0x01);
        // set standby bit to 1
        el_cam_register_write_uint8(0x34,0x01);
        // set target to register group A
        el_cam_register_write_uint8(0x03,0x00);

        e_i2cp_disable();
#endif

        el_cam_x = 0;
        el_cam_y = 0;
        el_cam_line_pointer = (uint16_t*)el_cam_w_frame->data;
        el_cam_pixel_pointer = (uint16_t*)el_cam_w_frame->data;

        el_cam_enabled = 0;
    }
}
/*
void el_cam_swap_buffer(){
    el_camera_image* temp;

    temp = el_cam_w_frame;
    el_cam_w_frame = el_cam_r_frame;
    el_cam_r_frame = temp;

}
*/
void el_camera_lock_frame(){
    el_cam_lock_buffer = 1;
}

void el_camera_unlock_frame(){
    el_cam_lock_buffer = 0;
}

bool el_camera_is_frame_locked(){
    return el_cam_lock_buffer;
}

uint16_t el_camera_get_frame_counter(){
    return el_cam_frame_counter;
}

el_camera_image*el_camera_get_frame(){
    return el_cam_r_frame;
}

void el_camera_get_frame_pixel(int X,int Y,uint8_t*rgb3v){
    const uint16_t red_bits   = 0b1111100000000000;
    const uint16_t green_bits = 0b0000011111100000;
    const uint16_t blue_bits  = 0b0000000000011111;
    uint16_t w = el_cam_r_frame->data[Y][X];
    rgb3v[0] = (w&red_bits)>>8;
    rgb3v[1] = (w&green_bits)>>3;
    rgb3v[2] = (w&blue_bits)<<3;
}

unsigned int el_camera_get_frame_width(){
    return el_cam_r_frame->width;
}

unsigned int el_camera_get_frame_height(){
    return el_cam_r_frame->height;
}
