
#ifndef EL_IR_PROXIMITY_H
#define EL_IR_PROXIMITY_H

#include "el_common.h"

#define EL_IR_PROXIMITY_MODE_PASSIVE     0
#define EL_IR_PROXIMITY_MODE_PULSE       1
#define EL_IR_PROXIMITY_MODE_EMIT        2
#define EL_IR_PROXIMITY_MODE_NOISE       3

EL_API void el_config_ir_proximity(el_enum mode);
EL_API void el_enable_ir_proximity(void);
EL_API void el_disable_ir_proximity(void);

#define EL_IR_PROXIMITY_MAX   4095

#define EL_IR_PROXIMITY_GET_AMBIENT      0
#define EL_IR_PROXIMITY_GET_REFLECTION   1
#define EL_IR_PROXIMITY_GET_NOISE        2

EL_API int el_ir_proximity_get(el_enum type,int which);
EL_API void el_ir_proximity_get_all(el_enum type,int*result_8v);
EL_API int el_ir_proximity_get_counter();


//==============================================================================


#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"
#include "el_trigger.h"

extern bool el_irps_enabled;
extern uint8_t el_irps_working_mode;
extern uint8_t el_irps_working_phase;
extern void (*el_adc_callback_ir_proximity)(const unsigned int*result_8v);
extern uint16_t el_irps_counter;
extern bool el_irps_is_calibrated;
extern uint16_t el_irps_environment_ambient;

extern uint16_t el_irps_samples_Neutral[8];
extern uint16_t el_irps_samples_Ambient[8];//Noise + Ambient
extern uint16_t el_irps_samples_Mixed[8];//Noise + Ambient + Emitter Reflection

extern uint16_t el_irps_samples_Temp[8];
extern uint16_t el_irps_samples_Last[8];
extern uint16_t el_irps_samples_Noise[8];
extern uint32_t el_irps_samples_Spikes[8];

void el_init_ir_proximity(void);
void el_routine_ir_proximity_2400hz(void);

#endif

#endif
