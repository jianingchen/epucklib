
#ifndef EL_LED_H
#define EL_LED_H

#include "el_common.h"

#define EL_LED_OFF      0
#define EL_LED_ON       1
#define EL_LED_TOGGLE   2

#define EL_LED_RING_0   0
#define EL_LED_RING_1   1
#define EL_LED_RING_2   2
#define EL_LED_RING_3   3
#define EL_LED_RING_4   4
#define EL_LED_RING_5   5
#define EL_LED_RING_6   6
#define EL_LED_RING_7   7
#define EL_LED_RING_ALL 8

#define EL_LED_BODY     9
#define EL_LED_FRONT    10

EL_API void el_led_set(int which,int action);

#endif
