
/*!

\defgroup EL_TIMER Timer

\section Introduction

Timer is the object associated with many features involving timing.

\section Usage

Call the function "el_create_timer" to create a timer in the system. 
Use the handle returned by this function to refer to the created timer 
in the related function.

Example 1 & 2 in the repository also demonstrate how to use timers.

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

/*!
    \brief create a timer in the system
    
    \return handle of the timer

    This function create a timer in the system. 
    It returns a handle to the timer  created. This handle is used 
    to refer the timer in the related functions.

    This function returns NULL if the creation is failed. 
*/
el_handle el_create_timer(void);


/*!
    \brief delete the timer

    \param h       handle of the timer
*/
void el_delete_timer(el_handle h);


/*!
    \brief set whether the timer repeat the countdown process once a countdown is finished

    \param h       handle of the timer
    \param b       is perodic or not
*/
void el_timer_set_perodic(el_handle h,bool b);


/*!
    \brief set the callback of a timer

    \param h       handle of the timer
    \param func    pointer to the function to be called when the timer expired
    \param arg     the argument passed to the callback function when it is called
    
    When the timer's countdown is finished, the time's callback "func" will be 
    called as "func(arg);". This callback need to be a routine function, which 
    means no wait/delay inside or any code with high computational cost. 
    
    A perodic timer with a callback can achieve the similar functionalility of 
    the 'agenda' in the official e-puck library, which creates a perodic 
    routine in the system. 
    
    Use NULL to not using any callback function. 
    By default, the timer does not has a callback function. 
*/
void el_timer_set_callback(el_handle h,el_timer_callback func,void*arg);


/*!
    \brief specify the countdown time and start the timer

    \param h       handle of the timer
    \param t_ms    period in millisecond
*/
void el_timer_start(el_handle h,el_time t_ms);


/*!
    \brief specify the countdown time using a fraction and start the timer

    \param h       handle of the timer
    \param num     numerator of the period in second
    \param den     denominator of the period in second
    
    The countdown time of the timer is (num/den) sec.
*/
void el_timer_start_fraction(el_handle h,int num,int den);// (num/den) second


/*!
    \brief pause the countdown of the timer 

    \param h       handle of the timer
*/
void el_timer_pause(el_handle h);


/*!
    \brief resume the countdown of the timer 

    \param h       handle of the timer
    
    This only applies to a started timer. 
*/
void el_timer_resume(el_handle h);


/*!
    \brief get the handle of the timer in a timer callback

    \return handle of the timer

    This function must be used within a timer callback.
*/
el_handle el_timer_callback_get_handle();


/*!
    \brief get the rounds of the timer 

    \param h       handle of the timer
    
    \return number of times that the countdown has been finished
    
    This function returns the number of times that the countdown has been finished. 
    Each time the timer's countdown finishes, its 'rounds' will +1. 
    Thus, for a on-going timer that is perodic (set via "el_timer_set_perodic"), 
    its rounds will keep increasing. 
*/
uint16_t el_timer_get_rounds(el_handle h);


/*!
    \brief set the rounds of the timer 

    \param h       handle of the timer
    \param n       rounds to be used
    
    This function is used to manually modify the rounds of a timer. 
    It is mainly used to reset the rounds of a timer to 0. 
*/
void el_timer_set_rounds(el_handle h,uint16_t n);



#ifdef EL_INCLUDE_CONTEXT

#define EL_TIMER_DIM      10

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
