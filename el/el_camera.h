
/*!

\defgroup EL_CAMERA Camera

\section Introduction

In this library, the resolution of the image captured by the camera 
is fixed to 40x15 with a frame rate of 18.432. 
This resolution is a result of subsampling, which means field of view (FOV) 
of the image is not lost. The horizontal FOV of the camera is about 56 deg 
while the vertical FOV is about 42 deg. Both FOV are almost fully covered in 
the captured image. 

The camera module in this library uses dual-buffer-swapping mechanism. At any moment, 
data acquired from the image sensor are written to frame buffer A (the front buffer) 
while any reading functions read data from frame buffer B (the back buffer). 
Once a frame is fully written, pointers to the two buffers will be swapped. 
This mechanism enables the frame to be processed concurrently with the acquisition 
of next frame. 


\section Usage

To run something when a frame of image is ready, create a trigger 
with EL_EVENT_CAMERA_FRAME_UPDATE event. 

Before reading data in the frame buffer, it (the back buffer) needs 
to be locked. The following mechanism is a typical loop to process a frame:
\code
    ...
    el_uint8 RGB[3];
    int X,Y;
    ...
    el_camera_lock_frame();
    for(X=0;X<EL_CAMERA_FRAME_DIM_X;X++){
        for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){
        
            el_camera_get_frame_pixel(X,Y,RGB);
            
            // image process algorithm
            ...
        }
    }
    el_camera_unlock_frame();
    ...
\endcode
When the frame buffer is locked, no buffer swapping will not occur. Therefore, to get 
a full frame rate of 18.432, the time cost of the image processing code needs to be 
less than the update interval of the frames. 

See Example 2 for a systematic usage of the camera module and image processing techniques. 

*/
 
/*!

\file
\ingroup EL_CAMERA

\author Jianing Chen

*/

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

#define EL_CAMERA_FRAME_DIM_X   40
#define EL_CAMERA_FRAME_DIM_Y   15

typedef struct DCIM{
    el_uint16 width;
    el_uint16 height;
    el_uint16 data[EL_CAMERA_FRAME_DIM_Y][EL_CAMERA_FRAME_DIM_X];
} el_camera_image;

void el_config_camera(el_camera_ini*setting);
void el_enable_camera();
void el_disable_camera();

void el_camera_lock_frame();
void el_camera_unlock_frame();
bool el_camera_is_frame_locked();
el_uint16 el_camera_get_frame_counter();
void el_camera_get_frame_pixel(int X,int Y,uint8_t*rgb3v);
unsigned int el_camera_get_frame_width();
unsigned int el_camera_get_frame_height();
el_camera_image*el_camera_get_frame();



#ifdef EL_INCLUDE_CONTEXT

#define EL_CAM_I2C_ID   0xDC

extern el_uint16 el_cam_device_id;
extern el_uint16 el_cam_revision_n;
extern uint8_t el_cam_auto_function;

extern el_camera_image el_frame_buffer_a;
extern el_camera_image el_frame_buffer_b;
extern el_camera_image *el_cam_r_frame;
extern el_camera_image *el_cam_w_frame;
extern el_uint16 *el_cam_line_pointer;
extern el_uint16 *el_cam_pixel_pointer;
extern el_uint16 el_cam_x;
extern el_uint16 el_cam_y;
extern volatile bool el_cam_lock_buffer;
extern volatile unsigned int el_cam_frame_counter;

void el_init_camera(void);
void el_cam_init_register(void);
void el_cam_swap_buffer(void);

#endif	/* EL_INCLUDE_CONTEXT */

#endif	/* EL_CAMERA_H */
