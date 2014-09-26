
#ifndef EL_CAMERA_H
#define	EL_CAMERA_H

#include <stdint.h>

#define EL_FRAME_BUFFER_WIDTH  40
#define EL_FRAME_BUFFER_HEIGHT 15

typedef struct DCIM{
    uint16_t dim_x;
    uint16_t dim_y;
    uint16_t RawData[EL_FRAME_BUFFER_HEIGHT][EL_FRAME_BUFFER_WIDTH];
} el_camera_image;

typedef struct EL_CAMERA_INI{
    bool AutoWhiteBalance;
    bool AutoExposure;
    float ExposureTime;
    float LinearGain;
    float RedGain;
    float GreenGain;
    float BlueGain;
} el_camera_ini;

extern const uint16_t RED_BITS;
extern const uint16_t GREEN_BITS;
extern const uint16_t BLUE_BITS;

EL_API void el_apply_camera_setting(el_camera_ini *setting);
EL_API void el_enable_camera();
EL_API void el_disable_camera();
EL_API void el_camera_lock_frame();
EL_API void el_camera_unlock_frame();
EL_API uint16_t el_camera_get_frame_counter();
EL_API el_camera_image*el_camera_get_frame();



#ifdef EL_INCLUDE_CONTEXT

#define EL_CAM_I2C_ID   0xDC 

extern el_camera_image el_frame_buffer_a;
extern el_camera_image el_frame_buffer_b;
extern el_camera_image *el_reading_frame;
extern el_camera_image *el_writing_frame;
extern uint16_t *el_cam_line;
extern uint16_t *el_cam_pixel;
extern uint16_t el_cam_x;
extern uint16_t el_cam_y;
extern uint16_t el_cam_byte_high;
extern uint16_t el_cam_byte_low;
extern volatile bool el_cam_lock_buffer;
extern volatile unsigned int el_cam_frame_counter;

void el_init_camera(void);
void el_cam_init_register(void);
void el_cam_swap_buffer(void);

#endif	/* EL_INCLUDE_CONTEXT */




#endif	/* EL_CAMERA_H */
