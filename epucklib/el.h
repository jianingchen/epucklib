/**

This is the main header of the library

**/
#ifndef EL_H
#define EL_H

#include "el_common.h"
#include "el_clock.h"
#include "el_random.h"
#include "el_list.h"
#include "el_timer.h"
#include "el_trigger.h"

#include "el_led.h"
#include "el_stepper_motor.h"
#include "el_camera.h"
#include "el_ir_receiver.h"
#include "el_accelerometer.h"
#include "el_ir_proximity.h"
#include "el_uart.h"

EL_API void el_initialization();
EL_API void el_enable(el_enum which);
EL_API void el_disable(el_enum which);
EL_API void el_sleep(el_time time_ms);// cannot be used in a process
EL_API int el_get_selector_value();
EL_API void el_main_loop();
EL_API void el_reset();

#endif
