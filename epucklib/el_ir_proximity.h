
#ifndef EL_IR_PROXIMITY_H
#define EL_IR_PROXIMITY_H

#define EL_IR_PROXIMITY_PULSE   0
#define EL_IR_PROXIMITY_KEEP    1
#define EL_IR_PROXIMITY_NOISE   2

#define EL_IR_PROXIMITY_MAX   4095

EL_API void el_enable_ir_proximity(void);
EL_API void el_disable_ir_proximity(void);





#ifdef EL_INCLUDE_CONTEXT

extern uint8_t el_irps_working_mode;
extern bool el_irps_enabled;

extern uint16_t el_irps_environment;
extern uint16_t el_irps_adc_counter;
extern uint16_t el_irps_counter;
extern bool el_irps_is_in_sampling_routine;
extern volatile bool el_irps_is_waiting_result;
extern volatile el_ct el_irps_micro_timer;
extern bool el_irps_is_calibrated;

extern uint16_t ProximitySensorsNeutral[8];
extern uint16_t ProximitySensorsAmbient[8];//Noise + Ambient
extern uint16_t ProximitySensorsMixed[8];//Noise + Ambient + Emitter Reflection

extern uint16_t ProximitySensorsTemp[8];
extern uint16_t ProximitySensorsLast[8];
extern uint16_t ProximitySensorsNoise[8];
extern uint32_t ProximitySensorsSpikes[8];

void el_init_ir_proximity();
void el_routine_ir_proximity();

#endif

#endif
