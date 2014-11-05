
/*!

\defgroup EL_IR_PROXIMITY Infrared Proximity Sensors

\section Introduction

(TODO)

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

#define EL_IR_PROXIMITY_OUTPUT_MAX   4095
#define EL_IR_PROXIMITY_SENSOR_0    0
#define EL_IR_PROXIMITY_SENSOR_1    1
#define EL_IR_PROXIMITY_SENSOR_2    2
#define EL_IR_PROXIMITY_SENSOR_3    3
#define EL_IR_PROXIMITY_SENSOR_4    4
#define EL_IR_PROXIMITY_SENSOR_5    5
#define EL_IR_PROXIMITY_SENSOR_6    6
#define EL_IR_PROXIMITY_SENSOR_7    7

/*! 
    This enum is used in ::el_config_ir_proximity to select the parameter 
    to be changed. 
*/
typedef enum{
    EL_WORKING_MODE,            ///< change the working mode
    EL_ENVIRONMENT_AMBIENT,     ///< change the ambient IR intensity in the environment
} el_ir_proximity_param;


/*! 
    This enum is used in ::el_config_ir_proximity to specify the working mode 
    of the IR proximity sensors. 
    
    Explanation for each of the working mode can be found in the introduction 
    section of \ref EL_IR_PROXIMITY. 
*/
typedef enum{
    EL_IR_PROXIMITY_PASSIVE = 0,    ///< use the Passive Mode
    EL_IR_PROXIMITY_PULSE = 1,      ///< use the Pulse Mode (default mode)
    EL_IR_PROXIMITY_EMIT = 2,       ///< use the Emit Mode
    EL_IR_PROXIMITY_NOISE = 3,      ///< use the Noise Mode (not implemented yet)
} el_ir_proximity_working_mode;


/*! 
    This enum is used in ::el_ir_proximity_get and ::el_ir_proximity_get to 
    select the output type to get.
    
    Explanation for each of output group can be found in the introduction 
    section of \ref EL_IR_PROXIMITY. 
*/
typedef enum{
    EL_IR_AMBIENT,              ///< get the ambient output
    EL_IR_REFLECTION,           ///< get the reflection output
    EL_IR_NOISE,                ///< get the noise output
} el_ir_proximity_output;


/*! 
    \brief configure various parameters of the IR proximity sensors module

    \param param    target parameter
    \param value    value to be applied
    
*/
void el_config_ir_proximity(el_ir_proximity_param param,int value);


/*! 
    \brief enable the proximity sensors module

*/
void el_enable_ir_proximity(void);


/*! 
    \brief disable the proximity sensors module

*/
void el_disable_ir_proximity(void);


/*! 
    \brief get a type of output value from a ir proximity sensor

    \param i        index of the sensor
    \param g        output type
    
    \return the output value
*/
int el_ir_proximity_get(el_index i,el_ir_proximity_output g);


/*! 
    \brief get a type of output value from all ir proximity sensors

    \param g        output type
    \param out8v    an int array to store the eight output values. 
    
*/
void el_ir_proximity_get_all(el_ir_proximity_output g,int*out8v);


/*! 
    \brief get total number of samples obtained since initialization
    
    \return number of samples
    
    When enabled, sampling frequency of the ir proximity sensors are 30Hz.
*/
int el_ir_proximity_get_counter();

//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

extern bool el_irps_enabled;
extern uint8_t el_irps_working_mode;
extern uint8_t el_irps_working_phase;
extern void (*el_adc_callback_ir_proximity)(const unsigned int*result_8v);
extern uint16_t el_irps_counter;

extern bool el_irps_is_calibrated;
extern uint16_t el_irps_environment_ambient;

extern uint16_t el_irps_samples_Ambient[8];//Noise + Ambient
extern uint16_t el_irps_samples_Mixed[8];//Noise + Ambient + Emitter Reflection
extern uint16_t el_irps_samples_Temp[8];
extern uint16_t el_irps_samples_Last[8];
extern uint32_t el_irps_samples_Spikes[8];
extern uint16_t el_irps_samples_Noise[8];
extern uint16_t el_irps_samples_NeutralReflection[8];// need calibration
extern uint16_t el_irps_samples_NeutralNoise[8];

void el_init_ir_proximity(void);
void el_routine_ir_proximity_2400hz(void);

#endif

#endif
