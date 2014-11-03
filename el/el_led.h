
#ifndef EL_LED_H
#define EL_LED_H

#include "el_common.h"

#define EL_LED_RING_ALL -1
#define EL_LED_RING_0   0
#define EL_LED_RING_1   1
#define EL_LED_RING_2   2
#define EL_LED_RING_3   3
#define EL_LED_RING_4   4
#define EL_LED_RING_5   5
#define EL_LED_RING_6   6
#define EL_LED_RING_7   7
#define EL_LED_BODY     9
#define EL_LED_FRONT    10

typedef enum{
    EL_OFF = 0,
    EL_ON = 1,
    EL_TOGGLE = -1,
} el_led_action;

void el_led_set(el_index w,el_led_action a);

int el_get_selector_value();

#endif
