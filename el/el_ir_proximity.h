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
\ingroup EL_IR_PROXIMITY

\author Jianing Chen

*/

#ifndef EL_IR_PROXIMITY_H
#define EL_IR_PROXIMITY_H

#include "el_common.h"
#include "el_clock.h"
#include "el_trigger.h"

#define EL_IR_PROXIMITY_VALUE_MAX   4095U

#define EL_IR_PROXIMITY_SENSOR_ALL  -1
#define EL_IR_PROXIMITY_SENSOR_0    0
#define EL_IR_PROXIMITY_SENSOR_1    1
#define EL_IR_PROXIMITY_SENSOR_2    2
#define EL_IR_PROXIMITY_SENSOR_3    3
#define EL_IR_PROXIMITY_SENSOR_4    4
#define EL_IR_PROXIMITY_SENSOR_5    5
#define EL_IR_PROXIMITY_SENSOR_6    6
#define EL_IR_PROXIMITY_SENSOR_7    7

/*! 
    This enum is used in \ref el_ir_proximity_param to specify the working mode
    of the IR proximity sensors. 
    
    Explanation for each of the working mode can be found in the introduction 
    section of \ref EL_IR_PROXIMITY. 
*/
typedef enum {
    EL_IR_PROXIMITY_PASSIVE = 0,        ///< use the Passive Mode
    EL_IR_PROXIMITY_PULSE = 1,          ///< use the Pulse Mode (default mode)
    EL_IR_PROXIMITY_EMIT = 2,           ///< use the Emit Mode
    EL_IR_PROXIMITY_COMMUNICATION = 3,  ///< use the Communication Mode (not implemented yet)
} el_ir_proximity_mode;


/*!
    This data structure is used in ::el_config_ir_proximity.
    A static instance of this struct exists internally, it can be pointed to
    through ::el_ir_proximity_options. 
*/
typedef struct {
    el_ir_proximity_mode WorkingMode;///< the working mode of the proximity sensors
    el_uint16 EnvironmentalAmbient;///< the ambient infrared intensity in the environment, used to correct the the ambient output
    el_bool SubtractEA;     ///< whether to use EnvironmentalAmbient in the ambient output
} el_ir_proximity_param;


/*! 
    This enum is used in ::el_ir_proximity_get to select the output type to get.
    
    Explanation for each of output group can be found in the introduction 
    section of \ref EL_IR_PROXIMITY. 
*/
typedef enum {
    EL_IR_AMBIENT,              ///< get the ambient output
    EL_IR_REFLECTION,           ///< get the reflection output
    EL_IR_DATA,                 ///< get the data encoded (not implemented yet)
    EL_IR_ALL_3V,               ///< get all of the three outputs
} el_ir_proximity_output;

/*!
    This struct can be used in ::el_ir_proximity_get when all types of outputs
    from all sensors need to be obtained. 
*/
typedef struct {
    el_int16 Ambient;       ///< This value can be used to detect nearby e-pucks in the Emit Mode.
    el_int16 Reflection;    ///< In all modes, this value can be used to detect passive things in the evnironment (e.g. obstacles and walls).
    el_int16 Message;       ///< (not implemented yet)
} el_ir_proximity_data;

/*
--------------------------------------------------------------------------------
*/


/*! 
    \brief enable the proximity sensors module

*/
void el_enable_ir_proximity(void);


/*! 
    \brief disable the proximity sensors module

*/
void el_disable_ir_proximity(void);


/*!
    \brief get the pointer to a static instance of \ref el_ir_proximity_param

    \return the pointer
*/
el_ir_proximity_param* el_ir_proximity_options();

/*!
    \brief apply the settings stored in ::el_ir_proximity_options directly

    This function is equivalent to "el_config_ir_proximity( el_ir_proximity_options() )".
*/
void el_ir_proximity_options_apply();


/*!
    \brief reset the settings stored in ::el_ir_proximity_options to default

    This function can be used in combination with ::el_ir_proximity_options_apply 
    when the default configuration need to be used. 
*/
void el_ir_proximity_options_reset();


/*!
    \brief apply the settings in the given structure

    \param p        pointer to the data structure

    The default setting of the ir proximity sensors can be represented by
    following code:
    \code
    el_ir_proximity_options()->WorkingMode = EL_IR_PROXIMITY_PULSE;
    el_ir_proximity_options()->EnvironmentalAmbient = 0;
    el_ir_proximity_options()->SubtractEA = false;
    el_config_ir_proximity( el_ir_proximity_options() );
    \endcode
*/
void el_config_ir_proximity(const el_ir_proximity_param*p);


/*!
    \brief get output value(s) from a accelerometer

    \param i        index of the sensor
    \param u        output type (see \ref el_ir_proximity_output)
    \param out      pointer to a variable or an array of variables to store
                    the output value(s).
    
    \return         number of values stored into *out.
    
    See \ref PAGE_COMP_INDEX for the index of each of the proximity sensors. 
    
    The dimension the data pointed by "out" depends on the number of output 
    values. For example, when one type of output of all eight proximity sensors 
    need to be stored, "out" needs to point to an array of eight el_int16.
    \code
    el_int16 ir_reflection[8];
    ...
    el_ir_proximity_get( EL_IR_PROXIMITY_SENSOR_ALL, EL_IR_REFLECTION, ir_reflection );
    ...
    \endcode
    
    When all types of outputs of all eight proximity sensors need to be stored, 
    An array of \ref el_ir_proximity_data should be used because the output 
    values will be interlaced. For example: 
    \code
    el_ir_proximity_data prox[8];
    ...
    el_ir_proximity_get( EL_IR_PROXIMITY_SENSOR_ALL, EL_IR_ALL_3V, (el_int16*)prox );
    ...
    \endcode
    
    At present, the refreshing rate of the proximity sensors are 30 Hz, 
    regardless of the working mode. Thus, a trigger with the event 
    ::EL_EVENT_IR_PROXIMITY_UPDATE will be triggered 30 times per second.
    
*/
int el_ir_proximity_get(el_index i,el_ir_proximity_output u,el_int16*out);


/*! 
    \brief get total number of samples obtained since initialization
    
    \return     number of samples
    
    When enabled, sampling frequency of the ir proximity sensors are 30Hz.
*/
el_uint32 el_ir_proximity_get_counter();


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

extern bool el_irps_enabled;
extern uint8_t el_irps_working_mode;
extern uint8_t el_irps_working_phase;
extern void (*el_adc_callback_ir_proximity)(const unsigned int*result_8v);

extern el_bool el_irps_is_calibrated;

extern el_uint16 el_irps_samples_Ambient[8];
extern el_uint16 el_irps_samples_Mixed[8];
extern el_uint16 el_irps_samples_Temp[8];
extern el_uint16 el_irps_samples_Noise[8];
extern el_uint16 el_irps_samples_NeutralReflection[8];// need calibration
extern el_uint16 el_irps_samples_NeutralNoise[8];

void el_init_ir_proximity(void);
void el_routine_ir_proximity_2400hz(void);

#endif

#endif
