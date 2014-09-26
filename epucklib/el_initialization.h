
#ifndef EL_INITIALIZATION_H
#define EL_INITIALIZATION_H

#define EL_RING_LED_0   0
#define EL_RING_LED_1   1
#define EL_RING_LED_2   2
#define EL_RING_LED_3   3
#define EL_RING_LED_4   4
#define EL_RING_LED_5   5
#define EL_RING_LED_6   6
#define EL_RING_LED_7   7
#define EL_RING_LED_ALL 8

#define EL_OFF      0
#define EL_ON       1
#define EL_TOGGLE   2

#define EL_BODY_LED     9
#define EL_FRONT_LED    10

#define EL_LEFT_WHEEL   0
#define EL_RIGHT_WHEEL  1
#define EL_BOTH_WHEEL   2

#define el_nop() {__asm__ volatile ("nop");}

EL_API void el_initialization();
EL_API void el_polled_wait(el_time time_ms);// cannot be used in process 
EL_API void el_reset_chip();
EL_API void el_set_led(int which,int action);
EL_API int el_get_selector_value();
EL_API void el_main_loop();

#endif
