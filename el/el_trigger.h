/*

embedded system library for e-puck

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

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
    This enum is used in ::el_trigger_set_event to specify the event 
    of a trigger. Internal events are manually triggered using
    ::el_trigger_issue_internal_event. 
*/
typedef enum {
    EL_EVENT_NONE = 0,          ///< no event
    EL_EVENT_INTERNAL_A = 1,    ///< internal event A
    EL_EVENT_INTERNAL_B = 2,    ///< internal event B
    EL_EVENT_INTERNAL_C = 3,    ///< internal event C
    EL_EVENT_INTERNAL_D = 4,    ///< internal event D
    EL_EVENT_INTERNAL_E = 5,    ///< internal event E
    EL_EVENT_INTERNAL_F = 6,    ///< internal event F
    EL_EVENT_IR_RECEIVER_INCOME = 10,   ///< when the ir receiver receives data
    EL_EVENT_UART1_RECEIVED = 11,       ///< when UART1 receives data
    EL_EVENT_UART2_RECEIVED = 12,       ///< when UART2 receives data (not implemented yet)
    EL_EVENT_ACCELEROMETER_UPDATE = 20, ///< when the accelerometer output is updated (120 Hz)
    EL_EVENT_IR_PROXIMITY_UPDATE = 21,  ///< when the ir proximity sensor outputs are updated (30 Hz)
    EL_EVENT_CAMERA_FRAME_UPDATE = 22,  ///< when a new image frame from the camera is ready (9~18 fps)
} el_trigger_event_type;

/*
--------------------------------------------------------------------------------
*/

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


/*!
    \brief check whether a trigger

    \param h    handle of the trigger
    
    \return the enabling status
*/
el_bool el_trigger_is_enabled(el_handle h);


/*! 
    \brief set the event of the trigger

    \param h    handle of the trigger
    \param e    type of the event within the category

    This function defines the event of the trigger. 
*/
void el_trigger_set_event(el_handle h,el_trigger_event_type e);


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

    \param h        handle of the trigger
    \param func     pointer to the process function

    When the trigger passes its condition and it has a registered process,
    the process will be launched while the trigger will be disabled 
    automatically. 
    
    The parameter of the process function of a trigger will be assigned with 
    the handle of the trigger. The following code can be used to make the handle
    more explicit: 
    \code
    void TriggerA_Process(void*arg){
        el_handle this_trigger = arg;
        
        // the program to be executed
        ...
        
        // re-enable the trigger if the trigger is supposed to be periodic. 
        el_trigger_enable(this_trigger);
        
    }
    \endcode
    
    To learn more about the usage of a process, see \ref EL_PROCESS.  
*/
void el_trigger_set_process(el_handle h,el_process func);


/*! 
    \brief get the number of times the trigger passed its condition

    \param h    handle of the trigger
    \return     counter of the trigger

    For each time the trigger passes its condition, its counter will +1. 
*/
el_uint32 el_trigger_get_counter(el_handle h);


/*! 
    \brief issue an internal event

    \param e    event type (must be internal events)

    This will affect those triggers using "EL_EVENT_INTERNAL_<*>" as their events. 
*/
void el_trigger_issue_internal_event(el_enum e);


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

#include "el_clock.h"

#define EL_TRIGGER_DIM   12

typedef struct{
    el_uint32 counter;
    el_enum event_type;
    void *event_data;
    el_condition condition;
    el_process process;
    bool enabled:1;
    bool remove:1;
    bool auto_disable:1;
    bool allow_recursive:1;
} el_trigger;

extern el_bool el_is_in_trigger_condition;
extern el_uint16 el_trigger_overwatch;
extern volatile el_uint8 el_trg_event_flag_in[6];
extern volatile el_uint8 el_trg_event_flag_ex_uart1;
extern volatile el_uint8 el_trg_event_flag_ex_uart2;
extern volatile el_uint8 el_trg_event_flag_ex_irrc;
extern volatile el_uint8 el_trg_event_flag_ex_cam;
extern volatile el_uint8 el_trg_event_flag_ex_acc;
extern volatile el_uint8 el_trg_event_flag_ex_irps;

void el_init_triggers();
void el_routine_triggers();

#endif

#endif	/* EL_TRIGGER_H */

