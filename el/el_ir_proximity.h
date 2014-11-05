
/*!

\defgroup EL_IR_PROXIMITY Infrared Proximity Sensor

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

typedef enum{
    EL_IR_PROXIMITY_MODE_PASSIVE,
    EL_IR_PROXIMITY_MODE_PULSE,
    EL_IR_PROXIMITY_MODE_EMIT,
    EL_IR_PROXIMITY_MODE_NOISE,
} el_ir_proximity_mode;

void el_config_ir_proximity(el_ir_proximity_mode mode);
void el_enable_ir_proximity(void);
void el_disable_ir_proximity(void);

#define EL_IR_PROXIMITY_OUTPUT_MAX   4095

typedef enum{
    EL_IR_AMBIENT,
    EL_IR_REFLECTION,
    EL_IR_NOISE,
} el_ir_proximity_output;

int el_ir_proximity_get(el_index which,el_ir_proximity_output type);
void el_ir_proximity_get_all(el_ir_proximity_output type,int*result_8v);
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
