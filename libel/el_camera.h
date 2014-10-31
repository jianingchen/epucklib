
#ifndef EL_CAMERA_H
#define	EL_CAMERA_H

#include "el_common.h"

#define EL_CAMERA_EXPOSURE_MODE_INTERNAL        0
#define EL_CAMERA_EXPOSURE_MODE_REGISTER        1
#define EL_CAMERA_EXPOSURE_MODE_EIT_X_LG        2
#define EL_CAMERA_EXPOSURE_MODE_EIT_X_GG        3

typedef struct EL_CAMERA_INI{
    el_enum ExposureMode;
    bool AutoWhiteBalance;
    bool AutoDigitalGain;
    float ExposureTime;
    float RedGain;
    float GreenGain;
    float BlueGain;
    float LinearGain;// LG
    float ExternalIntergationTime;// EIT
} el_camera_ini;

#define EL_CAMERA_FRAME_BUFFER_WIDTH  40
#define EL_CAMERA_FRAME_BUFFER_HEIGHT 15

typedef struct DCIM{
    uint16_t width;
    uint16_t height;
    uint16_t data[EL_CAMERA_FRAME_BUFFER_HEIGHT][EL_CAMERA_FRAME_BUFFER_WIDTH];
} el_camera_image;

void el_config_camera(el_camera_ini*setting);
void el_enable_camera();
void el_disable_camera();

void el_camera_lock_frame();
void el_camera_unlock_frame();
uint16_t el_camera_get_frame_counter();
void el_camera_get_frame_pixel(int X,int Y,uint8_t*rgb3v);
unsigned int el_camera_get_frame_width();
unsigned int el_camera_get_frame_height();
el_camera_image*el_camera_get_frame();

#ifdef EL_INCLUDE_CONTEXT

#define EL_CAM_I2C_ID   0xDC

extern el_camera_image el_frame_buffer_a;
extern el_camera_image el_frame_buffer_b;
extern el_camera_image *el_cam_r_frame;
extern el_camera_image *el_cam_w_frame;
extern uint16_t *el_cam_line_pointer;
extern uint16_t *el_cam_pixel_pointer;
extern uint16_t el_cam_x;
extern uint16_t el_cam_y;
extern volatile bool el_cam_lock_buffer;
extern volatile unsigned int el_cam_frame_counter;

void el_init_camera(void);
void el_cam_init_register(void);
void el_cam_swap_buffer(void);

#endif	/* EL_INCLUDE_CONTEXT */

#endif	/* EL_CAMERA_H */