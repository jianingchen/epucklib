
/*!

\defgroup EL_TIMER Timer

\section Introduction

Timer is the object used to deal with timing. 

\section Usage

Call the function "el_create_timer" to create a timer in the system. 
Use the handle returned by this function to refer to the created timer 
in the related function. 

*/
 
/*!

\file
\ingroup EL_TIMER

\author Jianing Chen

*/

#ifndef EL_TIMER_H
#define EL_TIMER_H

#include "el_common.h"
#include "el_clock.h"

typedef void (*el_timer_callback)(void*);

el_handle el_create_timer(void);
void el_delete_timer(el_handle h);
void el_timer_set_callback(el_handle h,el_timer_callback function,void*append_data);
void el_timer_set_perodic(el_handle h,bool is_perodic);
void el_timer_start(el_handle h,el_time time_ms);
void el_timer_start_fraction(el_handle h,int num,int den);// (num/den) second
void el_timer_pause(el_handle h);
void el_timer_resume(el_handle h);
uint16_t el_timer_get_rounds(el_handle h);
void el_timer_set_rounds(el_handle h,uint16_t n);



#ifdef EL_INCLUDE_CONTEXT

#include "el_list.h"

typedef struct{
    el_mct period;
    el_mct count_down;
    uint16_t rounds;
    void *append_data;
    void (*callback)(void*);
    uint8_t paused:1;
    uint8_t repeat:1;
    uint8_t remove:1;
} el_timer;

extern el_mci el_timer_mck;
extern bool el_is_in_timer_callback;

void el_init_timers();
void el_routine_timers();

#endif

#endif
