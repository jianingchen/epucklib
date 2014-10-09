
#ifndef EL_ACCELEROMETER_H
#define	EL_ACCELEROMETER_H

#include "el_common.h"

#ifdef EL_INCLUDE_CONTEXT

extern bool el_acc_enabled;
extern uint8_t el_acc_working_phase;
extern void (*el_adc_callback_accelerometer)(const unsigned int*result_3v);

void el_init_accelerometer();
void el_routine_accelerometer_2400hz(void);

#endif

#define EL_ACCELEROMETER_GET_X  0
#define EL_ACCELEROMETER_GET_Y  1
#define EL_ACCELEROMETER_GET_Z  2

EL_API void el_enable_accelerometer();
EL_API void el_disable_accelerometer();
EL_API int el_accelerometer_get(int which);
EL_API void el_accelerometer_get_all(int*result_3v);

#endif	/* EL_ACCELEROMETER_H */
