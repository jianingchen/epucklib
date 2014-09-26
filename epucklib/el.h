/**

This header is main api.

**/
#ifndef EL_H
#define EL_H

#include <stdlib.h>
#include <stdint.h>


#define EL_API

#define BIT(b) (1<<(b))

typedef unsigned short el_handle;
typedef unsigned char bool;
#define false 0
#define true 1


#include "el_list.h"
#include "el_clock.h"
#include "el_random.h"
#include "el_uart.h"
#include "el_timer.h"
#include "el_trigger.h"
#include "el_stepper_motor.h"
#include "el_camera.h"
#include "el_ir_receiver.h"
#include "el_ir_proximity.h"
#include "el_interrupt.h"
#include "el_initialization.h"


#endif
