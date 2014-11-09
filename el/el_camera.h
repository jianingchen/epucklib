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

/*!

\file
\ingroup EL_CAMERA

\author Jianing Chen

*/

#ifndef EL_CAMERA_H
#define	EL_CAMERA_H

#include "el_common.h"
#include "el_clock.h"

#define EL_CAMERA_FRAME_DIM_X   40
#define EL_CAMERA_FRAME_DIM_Y   15

/*!
    This enum is used in \ref el_camera_param to specify the exposure
    mode of the camera. 
*/
typedef enum {
    EL_AUTOMATIC = 0,       ///< automatically adjusted by the camera
    EL_EXPOSURE_TIME = 1,   ///< use ExposureTime
    EL_EIT_AND_ELG = 2,     ///< use ExternalIntergationTime and ExternalLinearGain
    EL_IT_AND_GG = 3,       ///< use IntegrationTime and GlobalGain
} el_camera_exposure_mode;


/*!
    This data structure is used in ::el_config_camera. 
    A static instance of this struct exists internally, it can be pointed to
    through ::el_config_camera_list. 

    Detailed explanation of these parameters can be found in the datasheet of
    PO6030K.
*/
typedef struct {
    el_camera_exposure_mode ExposureMode;
    el_bool AutoWhiteBalance;
    el_bool AutoDigitalGain;
    float ExposureTime;             ///< sensible range is [ 1.0, 20.0 ]
    float ExternalIntergationTime;  ///< valid range is ( 0.0, 512.0 )
    float ExternalLinearGain;       ///< valid range is ( 0.0, 256.0 )
    float IntegrationTime;          ///< valid range is ( 0.0, 512.0 )
    float GlobalGain;               ///< valid range is [ 1.0, 32.0 ]
    float DigitalGain;              ///< valid range is [ 0.015, 3.98 ]
    float RedGain;                  ///< valid range is [ 0.015, 3.98 ]
    float GreenGain;                ///< valid range is [ 0.015, 3.98 ]
    float BlueGain;                 ///< valid range is [ 0.015, 3.98 ]
} el_camera_param;


/*!
    This struct is used with ::el_camera_frame when the camera image needs 
    to be accessed directly. 
*/
typedef struct {
    el_uint16 Width;
    el_uint16 Height;
    el_uint16 Data[EL_CAMERA_FRAME_DIM_Y][EL_CAMERA_FRAME_DIM_X];
} el_camera_image;

/*
--------------------------------------------------------------------------------
*/

void el_enable_camera();

void el_disable_camera();


/*!
    \brief get the pointer to a static instance of \ref el_camera_param

    \return the pointer
*/
el_camera_param* el_config_camera_list();


/*!
    \brief apply the parameters to the camera

    \param p        pointer to the data structure

    The default parameters of the camera is equivalent to the following code:
    \code
    el_camera_param *CameraSetting;
    ...
    CameraSetting = el_config_camera_list();
    CameraSetting->ExposureMode = EL_AUTOMATIC;
    CameraSetting->AutoWhiteBalance = true;
    CameraSetting->AutoDigitalGain = true;
    CameraSetting->ExposureTime = 1.0f;
    CameraSetting->RedGain = 1.0f;
    CameraSetting->GreenGain = 1.0f;
    CameraSetting->BlueGain = 1.0f;
    CameraSetting->ExternalIntergationTime = 1.0f;
    CameraSetting->ExternalLinearGain = 1.0f;
    CameraSetting->IntegrationTime = 1.0f;
    CameraSetting->GlobalGain = 1.0f;
    CameraSetting->DigitalGain = 1.0f;
    el_config_camera( CameraSetting );
    \endcode
    
    The time cost to execute this function is not neglectable (around 7.1 ms), 
    so it should not be executed frequently in a process. 
*/
void el_config_camera(const el_camera_param*p);


/*!
    \brief lock the frame buffers
    
    When the frame buffer is locked, no buffer swapping will occur. This 
    guarantees the image inside the frame buffer to be one frame of image. 
    Otherwise, the image may be changed halfway through a image processing 
    algorithm. 
*/
void el_camera_lock_frame();


/*!
    \brief unlock the frame buffers

*/
void el_camera_unlock_frame();


/*!
    \brief check whether the frame buffers is locked
    
    \return     locked or not
*/
bool el_camera_is_frame_locked();



/*!
    \brief get total number of frames captured since initialization
    
    \return     total number of frames
    
    When the frame buffers are locked, the frame counter will not be increased. 
*/
el_uint32 el_camera_get_frame_counter();


/*!
    \brief get the color of a pixel in current frame buffer. 
    
    \param X        x coordinate of the pixel
    \param Y        y coordinate of the pixel
    \param out3v    pointer to the array to store the RGB components. 
    
    (0,0) is at bottom left. 
    
*/
void el_camera_get_frame_pixel(int X,int Y,el_uint8*out3v);


el_uint16 el_camera_get_frame_width();

el_uint16 el_camera_get_frame_height();


/*!
    \brief get the pointer to a static instance of \ref el_camera_image for reading

    \return the pointer
*/
el_camera_image*el_camera_frame();


/*
--------------------------------------------------------------------------------
*/


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
extern volatile el_uint16 el_cam_lock_buffer;
extern volatile el_uint32 el_cam_frame_counter;

void el_init_camera(void);
void el_cam_init_register(void);
void el_cam_swap_buffer(void);

#endif	/* EL_INCLUDE_CONTEXT */

#endif	/* EL_CAMERA_H */
