
#ifndef EL_TRIGGER_H
#define	EL_TRIGGER_H

typedef bool (*el_condition)(void*);
typedef void (*el_process)(void*);
#define EL_PROCESS void


#define EL_EVENT_IR_RECEIVER_INCOMING       1
#define EL_EVENT_IR_SENSOR_UPDATE           2
#define EL_EVENT_CAMERA_FRAME_UPDATE        3
#define EL_EVENT_UART1_INCOMING             11
#define EL_EVENT_UART2_INCOMING             12

EL_API int el_launch_process(el_process function,void*appended_data);
EL_API void el_process_cooperate();
EL_API void el_process_wait(el_time time_ms);

EL_API el_handle el_create_trigger();
EL_API int el_trigger_register_condition(el_handle trigger,el_condition function);
EL_API int el_trigger_register_process(el_handle trigger,el_process function);
EL_API int el_trigger_register_event_exclusive(el_handle trigger,int event_type);
EL_API int el_trigger_register_event_timing(el_handle trigger,el_time time,bool repeat);
EL_API int el_trigger_register_event_sound(el_handle trigger,int frequency);
EL_API void el_delete_trigger(el_handle trigger);

#ifdef EL_INCLUDE_CONTEXT

#include "cmt.h"

typedef struct EL_TRIGGER{
    uint16_t counter;
    el_condition condition;
    el_process process;
    struct EL_TRIGGER **event;
} el_trigger;

extern bool el_is_in_cmt_process;
extern el_mct el_triggers_mck;
extern el_mct el_triggers_mcr;

void el_init_triggers();
void el_routine_triggers();

#endif

#endif	/* EL_TRIGGER_H */

