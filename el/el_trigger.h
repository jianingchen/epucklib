
/*!

\defgroup EL_TRIGGER Trigger

\section Introduction

A trigger can launch a process when a specified event occurs in the system. 
For example, a process can be launched automatically when the proximity sensors has 
a set of samples ready for use. 


\section Usage

Call the function "el_create_trigger" to create a trigger in the system. 
Use the handle returned by this function to refer to the created trigger 
in the related function. 

*/
 
/*!

\file
\ingroup EL_TRIGGER

\author Jianing Chen

*/

#ifndef EL_TRIGGER_H
#define	EL_TRIGGER_H

#include "el_common.h"
#include "el_process.h"

typedef bool (*el_condition)(void*);

/*!
    \brief create a trigger in the system

    \return handle of the trigger

    This function create a trigger in the system. 
    It returns a handle to the trigger created. This handle is used 
    to refer the trigger in the related functions. 
*/
el_handle el_create_trigger();


/*!
    \brief delete the trigger

    \param handle of the trigger
*/
void el_delete_trigger(el_handle trigger);

/*! 
    \brief enable a trigger

    \param h    handle of the trigger

    Only when a trigger is enabled, its event will be effective.
    A trigger is enabled by default after its creation.
*/
void el_trigger_enable(el_handle h);

/*! 
    \brief disable a trigger

    \param h    handle of the trigger

    When a trigger is disabled, its event will be ignored. 
    A trigger is automatically disabled when its process is launched. 
*/
void el_trigger_disable(el_handle h);

#define EL_EVENT_NONE                       0
#define EL_EVENT_INTERNAL_A                 1
#define EL_EVENT_INTERNAL_B                 2
#define EL_EVENT_INTERNAL_C                 3
#define EL_EVENT_INTERNAL_D                 4
#define EL_EVENT_INTERNAL_E                 5
#define EL_EVENT_INTERNAL_F                 6
#define EL_EVENT_IR_RECEIVER_INCOME         10
#define EL_EVENT_UART1_ENTERED              11
#define EL_EVENT_UART2_ENTERED              12
#define EL_EVENT_ACCELEROMETER_UPDATE       20
#define EL_EVENT_IR_PROXIMITY_UPDATE        21
#define EL_EVENT_CAMERA_FRAME_UPDATE        22

/*! 
    \brief set the event of the trigger

    \param h    handle of the trigger
    \param e    type of the event within the category

    This function defines the event of the trigger. 
*/
void el_trigger_set_event(el_handle h,el_enum e);

/*! 
    \brief set the condition callback function of the trigger
    
    \param h    handle of the trigger
    \param f    pointer to the callback function
    
    When the event of the trigger occurs, the trigger's condition callback will
    be called. If this callback function returns true, the trigger passes the
    condition. Otherwise, the trigger does not pass the condition.

    If a trigger does not have a condition callback (by default), the trigger
    will automatically pass it's condition. 
*/
void el_trigger_set_condition(el_handle h,el_condition f);

/*!
    \brief set the process entry function of the trigger

    \param h    handle of the trigger
    \param f    pointer to the process function

    When the trigger passes its condition and it has a registered process,
    the process will be launched and the trigger will be disabled automatically.
*/
void el_trigger_set_process(el_handle h,el_process f);

/*! 
    \brief get the number of times the trigger passed its condition

    \param h    handle of the trigger
    \return counter of the trigger

    For each time the trigger passes its condition, its counter will +1. 
*/
uint16_t el_trigger_get_counter(el_handle h);

void el_trigger_issue_internal_event(el_enum e);



#ifdef EL_INCLUDE_CONTEXT

#include "el_clock.h"

#define EL_TRIGGER_DIM   16

typedef struct{
    uint16_t counter;
    el_enum event_type;
    void *event_data;
    el_condition condition;
    el_process process;
    bool enabled:1;
    bool remove:1;
    bool auto_disable:1;
    bool allow_recursive:1;
} el_trigger;

extern bool el_is_in_trigger_condition;
extern volatile uint8_t el_trg_event_flag_ex_uart1;
extern volatile uint8_t el_trg_event_flag_ex_uart2;
extern volatile uint8_t el_trg_event_flag_ex_irrc;
extern volatile uint8_t el_trg_event_flag_ex_cam;
extern volatile uint8_t el_trg_event_flag_ex_acc;
extern volatile uint8_t el_trg_event_flag_ex_irps;

void el_init_triggers();
void el_routine_triggers();

#endif

#endif	/* EL_TRIGGER_H */

