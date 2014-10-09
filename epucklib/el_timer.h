
#ifndef EL_TIMER_H
#define EL_TIMER_H

#include "el_common.h"

#ifdef EL_INCLUDE_CONTEXT

#include "el_list.h"
#include "el_clock.h"

typedef struct EL_TIMER{
    el_mct period;
    el_mct count_down;
    uint16_t rounds;
    void *append_data;
    void (*callback)(void*);
    uint8_t paused:1;
    uint8_t repeat:1;
    uint8_t remove:1;
} el_timer;

extern el_mct el_timer_mck;

void el_init_timers();
void el_routine_timers();

#endif

EL_API el_handle el_create_timer(void);
EL_API void el_delete_timer(el_handle h);
EL_API void el_timer_set_callback(el_handle h,void (*callback)(void*),void*append_data);
EL_API void el_timer_set_perodic(el_handle h,bool perodic);
EL_API void el_timer_start(el_handle h,el_time time_ms);
EL_API void el_timer_start_fraction(el_handle h,int num,int den);// (num/den) second
EL_API void el_timer_pause(el_handle h);
EL_API void el_timer_resume(el_handle h);
EL_API uint16_t el_timer_get_rounds(el_handle h);
EL_API void el_timer_set_rounds(el_handle h,uint16_t n);

#endif
