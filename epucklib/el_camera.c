
#include "el_context.h"
#include "el_camera.h"

el_camera_image *el_reading_frame;//for reading
el_camera_image *el_writing_frame;//for writing
el_camera_image el_frame_buffer_a;
el_camera_image el_frame_buffer_b;
uint8_t el_cam_margin[8];

uint16_t *el_cam_line;
uint16_t *el_cam_pixel;
uint16_t el_cam_x;
uint16_t el_cam_y;
uint16_t el_cam_byte_high;
uint16_t el_cam_byte_low;

bool el_cam_enabled;
volatile bool el_cam_lock_buffer;
volatile uint16_t el_cam_frame_counter;
uint16_t el_cam_device_id;
uint16_t el_cam_revision_n;
uint8_t el_cam_auto_function;

static uint16_t el_cam_register_read(uint8_t address){
    uint8_t H;
    uint8_t L;
    H = e_i2cp_read(EL_CAM_I2C_ID,address);
    L = e_i2cp_read(EL_CAM_I2C_ID,address+1);
    return (H<<8)|L;
}

static void el_cam_register_write(uint8_t address,uint16_t w){
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
    el_reading_frame = &el_frame_buffer_a;
    el_writing_frame = &el_frame_buffer_b;

    el_cam_device_id = 0;
    el_cam_revision_n = 0;
    el_cam_auto_function = 0;

    el_frame_buffer_a.dim_x = 0;
    el_frame_buffer_a.dim_y = 0;
    el_frame_buffer_b.dim_x = 0;
    el_frame_buffer_b.dim_y = 0;
    for(Y=0;Y<EL_FRAME_BUFFER_HEIGHT;Y++){
        for(X=0;X<EL_FRAME_BUFFER_WIDTH;X++){
            el_frame_buffer_a.RawData[Y][X] = 0;
            el_frame_buffer_b.RawData[Y][X] = 0;
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
    el_cam_device_id = el_cam_register_read(0x00);
    el_cam_revision_n = el_cam_register_read(0x02);

    // set target to register group B
    e_i2cp_write(EL_CAM_I2C_ID,0x03,0x01);

    // change clock rate
    e_i2cp_write(EL_CAM_I2C_ID,0x38,0x08);// color format
    e_i2cp_write(EL_CAM_I2C_ID,0x68,0x01);// pixel clock rate
    e_i2cp_write(EL_CAM_I2C_ID,0x51,0x04);
    e_i2cp_write(EL_CAM_I2C_ID,0x53,0x04);
    e_i2cp_write(EL_CAM_I2C_ID,0x54,0x01);
    e_i2cp_write(EL_CAM_I2C_ID,0x55,0x43);
    e_i2cp_write(EL_CAM_I2C_ID,0x56,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0x57,0xF3);
    e_i2cp_write(EL_CAM_I2C_ID,0x61,0x0C);
    e_i2cp_write(EL_CAM_I2C_ID,0x63,0xEC);
    e_i2cp_write(EL_CAM_I2C_ID,0x80,0x40);
    e_i2cp_write(EL_CAM_I2C_ID,0x81,0x40);
    e_i2cp_write(EL_CAM_I2C_ID,0x82,0x01);

    // set target to register group C
    e_i2cp_write(EL_CAM_I2C_ID,0x03,0x02);
    el_cam_auto_function = e_i2cp_read(EL_CAM_I2C_ID,0x04);

    // change image format to 320x240
    e_i2cp_write(EL_CAM_I2C_ID,0x11,0x12);
    e_i2cp_write(EL_CAM_I2C_ID,0x13,0x0E);
    e_i2cp_write(EL_CAM_I2C_ID,0x14,0x01);
    e_i2cp_write(EL_CAM_I2C_ID,0x15,0x30);
    e_i2cp_write(EL_CAM_I2C_ID,0x16,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0x17,0xDF);
    e_i2cp_write(EL_CAM_I2C_ID,0x19,0x72);
    e_i2cp_write(EL_CAM_I2C_ID,0x1B,0x43);
    e_i2cp_write(EL_CAM_I2C_ID,0x1D,0x50);
    e_i2cp_write(EL_CAM_I2C_ID,0x1F,0x50);

    // set exposure control & white balance
    e_i2cp_write(EL_CAM_I2C_ID,0x04,0b10011010);//Auto White Balance ON + Auto Exposure Control OFF
    e_i2cp_write(EL_CAM_I2C_ID,0x24,0b00000000);//External Integration Time H
    e_i2cp_write(EL_CAM_I2C_ID,0x25,0b10000000);//External Integration Time M
    e_i2cp_write(EL_CAM_I2C_ID,0x26,0b00000000);//External Integration Time L
    el_cam_register_write(0x28,0x1000);//External Linear Gain
    e_i2cp_write(EL_CAM_I2C_ID,0x55,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0x56,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0xA4,64*1.0000);//Red Gain
    e_i2cp_write(EL_CAM_I2C_ID,0xA5,64*1.0000);//Green Gain
    e_i2cp_write(EL_CAM_I2C_ID,0xA6,64*1.0000);//Blue Gain
    //e_i2cp_write(EL_CAM_I2C_ID,0xA6,0b01001111);//Blue Gain = (fix point)1.234375

    // set target to register group A
    e_i2cp_write(EL_CAM_I2C_ID,0x03,0x00);

    e_i2cp_disable();
    
}

void el_apply_camera_setting(el_camera_ini *setting){
    uint8_t R = 64*setting->RedGain;
    uint8_t G = 64*setting->GreenGain;
    uint8_t B = 64*setting->BlueGain;
    uint16_t K = 4096*setting->LinearGain;
    
    union{
        uint32_t d;
        struct{
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        };
    } time;

    union{
        uint8_t all_bits;
        struct{
            uint8_t bit0_AutoWhiteBalance:1;
            uint8_t bit1_2_AutoExposureMode:2;
            uint8_t bit3_unused:1;
            uint8_t bit4_unused:1;
            uint8_t bit5_unused:1;
            uint8_t bit6_unused:1;
            uint8_t bit7_unused:1;
        };
    } flags;
    
    time.d = 32768;
    
    e_i2cp_enable();
    
    // set target to register group C
    e_i2cp_write(EL_CAM_I2C_ID,0x03,0x02);
    
    e_i2cp_write(EL_CAM_I2C_ID,0x04,flags.all_bits);
    e_i2cp_write(EL_CAM_I2C_ID,0x24,0b00000000);//External Integration Time H
    e_i2cp_write(EL_CAM_I2C_ID,0x25,0b10000000);//External Integration Time M
    e_i2cp_write(EL_CAM_I2C_ID,0x26,0b00000000);//External Integration Time L
    el_cam_register_write(0x28,K);//External Linear Gain
    e_i2cp_write(EL_CAM_I2C_ID,0x55,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0x56,0x00);
    e_i2cp_write(EL_CAM_I2C_ID,0xA4,R);//Red Gain
    e_i2cp_write(EL_CAM_I2C_ID,0xA5,G);//Green Gain
    e_i2cp_write(EL_CAM_I2C_ID,0xA6,B);//Blue Gain
    
    // set target to register group A
    e_i2cp_write(EL_CAM_I2C_ID,0x03,0x00);
    
    e_i2cp_disable();
    
}

void el_cam_swap_buffer(){
    el_camera_image* temp;
    
    temp = el_writing_frame;
    el_writing_frame = el_reading_frame;
    el_reading_frame = temp;
    
}

void el_enable_camera(){
    if(!el_cam_enabled){
        T5CONbits.TON = 1;
        el_cam_enabled = 1;
    }
}

void el_disable_camera(){
    if(el_cam_enabled){
        T5CONbits.TON = 0;
        T4CONbits.TON = 0;
        T1CONbits.TON = 0;
        el_cam_enabled = 0;
    }
}

void el_camera_lock_frame(){
    el_cam_lock_buffer = 1;
}

void el_camera_unlock_frame(){
    el_cam_lock_buffer = 0;
}

uint16_t el_camera_get_frame_counter(){
    return el_cam_frame_counter;
}

el_camera_image*el_camera_get_frame(){
    return el_reading_frame;
}