
#ifndef EL_TIMER_H
#define EL_TIMER_H

#ifdef EL_INCLUDE_CONTEXT

typedef struct EL_TIMER{
    el_time period;
    el_time time;
    int rounds;
    void *append_data;
    void (*callback)(void*);
    uint8_t paused:1;
    uint8_t repeat:1;
    uint8_t remove:1;
} el_timer;

extern el_mct el_timers_mck;
extern el_mct el_timers_mcr;

void el_init_timers();
void el_routine_timers();

#endif

EL_API el_handle el_create_timer(void);
EL_API void el_delete_timer(el_handle h);
EL_API void el_timer_set_callback(el_handle h,void (*callback)(void*),void*append_data);
EL_API void el_timer_set_start(el_handle h,el_time time_ms);
EL_API void el_timer_set_pause(el_handle h,el_time time_ms);
EL_API void el_timer_pause(el_handle h);
EL_API void el_timer_resume(el_handle h);
EL_API int el_timer_get_rounds(el_handle h);


#endif
