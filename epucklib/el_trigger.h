
#ifndef EL_TRIGGER_H
#define	EL_TRIGGER_H

#include "el_common.h"

typedef bool (*el_condition)(void*);
typedef void (*el_process)(void*);
#define EL_CONDITION bool
#define EL_PROCESS void

#ifdef EL_INCLUDE_CONTEXT

#include "el_list.h"
#include "el_clock.h"
#include "el_timer.h"
#include "cmt.h"

typedef struct EL_TRIGGER{
    uint16_t counter;
    el_enum event_type;
    void *event_data;
    el_condition condition;
    el_process process;
    bool enabled;
} el_trigger;

extern bool el_is_in_cmt_process;
extern el_mct el_trigger_mck;

void el_init_triggers();
void el_routine_triggers();

void el_trg_event_handler_irrc();
void el_trg_event_handler_acc();
void el_trg_event_handler_irps();
void el_trg_event_handler_cam();

#endif

EL_API int el_launch_process(el_process function,void*appended_data);
EL_API void el_process_cooperate();
EL_API void el_process_wait(el_time time_ms);

EL_API el_handle el_create_trigger();
EL_API void el_delete_trigger(el_handle trigger);

#define EL_EVENT_IR_RECEIVER_INCOMING       10
#define EL_EVENT_UART1_INCOMING             11
#define EL_EVENT_UART2_INCOMING             12
#define EL_EVENT_ACCELEROMETER_UPDATE       20
#define EL_EVENT_IR_PROXIMITY_UPDATE        21
#define EL_EVENT_CAMERA_FRAME_UPDATE        25
#define EL_EVENT_TIMING                     50

EL_API void el_trigger_enable(el_handle trigger);
EL_API void el_trigger_disable(el_handle trigger);
EL_API void el_trigger_register_condition(el_handle trigger,el_condition function);
EL_API void el_trigger_register_process(el_handle trigger,el_process function);
EL_API void el_trigger_register_exclusive_event(el_handle trigger,el_enum event_type);
EL_API void el_trigger_register_timing_event(el_handle trigger,el_time time,bool repeat);
EL_API uint16_t el_trigger_get_counter(el_handle trigger);

#endif	/* EL_TRIGGER_H */

