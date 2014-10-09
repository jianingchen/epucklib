
#include "el_context.h"
#include "el_trigger.h"

bool el_is_in_cmt_process;
el_mct el_trigger_mck;
//el_mct el_trg_exclusive_event_mcr;

el_list *el_trigger_list;
volatile bool el_trigger_list_lock;

el_trigger*el_trg_exclusive_event_ir_receiver;
el_trigger*el_trg_exclusive_event_uart1;
el_trigger*el_trg_exclusive_event_uart2;
el_trigger*el_trg_exclusive_event_camera_frame;
el_trigger*el_trg_exclusive_event_ir_proximity;
el_trigger*el_trg_exclusive_event_accelerometer;

void el_init_triggers(){
    cmt_ini cmt_setup;

    cmt_setup.MaxNumberOfProcess = 8;
    cmt_setup.ProcessStackSzie = 160;
    cmt_setup.ProcessStackOffset = 128;
    cmt_initialize(&cmt_setup);
    
    el_is_in_cmt_process = 0;
    
    el_trigger_list = el_create_list();
    el_trigger_list_lock = false;

    el_trg_exclusive_event_ir_receiver = NULL;
    el_trg_exclusive_event_uart1 = NULL;
    el_trg_exclusive_event_uart2 = NULL;
    el_trg_exclusive_event_camera_frame = NULL;
    el_trg_exclusive_event_ir_proximity = NULL;
    
    el_trigger_mck = el_get_masterclock();

}

void el_routine_triggers(){
    el_mct current_clock;
    el_ct dk;

    // calculate time difference
    current_clock = el_get_masterclock();
    dk = current_clock - el_trigger_mck;
    el_trigger_mck = current_clock;
    
    // process the cooperative multi-task system
    el_is_in_cmt_process = true;

    cmt_main_routine();
    cmt_process_timers(dk);

    el_is_in_cmt_process = false;

}

void el_process_wait(el_time time_ms){
    if(el_is_in_cmt_process){
        cmt_wait(EL_TIME_TO_MTK(time_ms));
    }
}

void el_process_cooperate(){
    if(el_is_in_cmt_process){
        cmt_cooperate();
    }
}

int el_launch_process(el_process function,void*appended_data){
    return cmt_launch_process_delay(function,appended_data,0);
}

el_handle el_create_trigger(){
    el_trigger *p;
    
    p = (el_trigger*)malloc(sizeof(el_trigger));
    
    p->counter = 0;
    p->enabled = true;
    p->event_type = 0;
    p->event_data = NULL;
    p->condition = NULL;
    p->process = NULL;
    
    return EL_POINTER_TO_HANDLE(p);
}

uint16_t el_trigger_get_counter(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    return p->counter;
}

void el_trigger_register_condition(el_handle trigger,el_condition function){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->condition = function;
}

void el_trigger_register_process(el_handle trigger,el_process function){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->process = function;
}

void el_trigger_register_exclusive_event(el_handle trigger,el_enum event_type){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    
    switch(event_type){
        
    case EL_EVENT_IR_RECEIVER_INCOMING:
        el_trg_exclusive_event_ir_receiver = p;
        p->event_type = event_type;
        break;
        
    case EL_EVENT_UART1_INCOMING:
        el_trg_exclusive_event_uart1 = p;
        p->event_type = event_type;
        break;
        
    case EL_EVENT_UART2_INCOMING:
        el_trg_exclusive_event_uart2 = p;
        p->event_type = event_type;
        break;
        
    case EL_EVENT_ACCELEROMETER_UPDATE:
        el_trg_exclusive_event_accelerometer = p;
        p->event_type = event_type;
        break;

    case EL_EVENT_IR_PROXIMITY_UPDATE:
        el_trg_exclusive_event_ir_proximity = p;
        p->event_type = event_type;
        break;

    case EL_EVENT_CAMERA_FRAME_UPDATE:
        el_trg_exclusive_event_camera_frame = p;
        p->event_type = event_type;
        break;
        
    default:
        p->event_type = 0;
        break;
    }
    
}

static void el_trg_procedure(el_trigger *trg){
    bool F;
    if(trg->enabled){
        if(trg->condition){
            F = trg->condition(trg);
        }else{
            F = true;
        }
        if(F){
            trg->counter++;
            if(trg->process){
                el_launch_process(trg->process,trg);
            }
        }
    }
}

void el_trg_event_handler_irrc(){
    if(el_trg_exclusive_event_ir_receiver){
        el_trg_procedure(el_trg_exclusive_event_ir_receiver);
    }
}

void el_trg_event_handler_acc(){
    if(el_trg_exclusive_event_accelerometer){
        el_trg_procedure(el_trg_exclusive_event_accelerometer);
    }
}

void el_trg_event_handler_irps(){
    if(el_trg_exclusive_event_ir_proximity){
        el_trg_procedure(el_trg_exclusive_event_ir_proximity);
    }
}

void el_trg_event_handler_cam(){
    if(el_trg_exclusive_event_camera_frame){
        el_trg_procedure(el_trg_exclusive_event_camera_frame);
    }
}

void el_trg_event_handler_timing(void*append_data){
    el_trigger *trg = EL_HANDLE_TO_POINTER(append_data);
    bool F;
    if(trg->enabled){
        if(trg->condition){
            F = trg->condition(trg);
        }else{
            F = true;
        }
        if(F){
            trg->counter++;
            if(trg->process){
                el_launch_process(trg->process,trg);
            }
        }
    }
}

void el_trigger_register_timing_event(el_handle trigger,el_time time,bool repeat){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    el_handle T;
    
    T = el_create_timer();
    el_timer_set_callback(T,el_trg_event_handler_timing,p);
    el_timer_set_perodic(T,repeat);
    
    p->event_type = EL_EVENT_TIMING;
    p->event_data = T;
    
    el_timer_start(T,time);
    if(!p->enabled){
        el_timer_pause(T);
    }
    
}

void el_trigger_enable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = true;
    if( p->event_type==EL_EVENT_TIMING){
        el_timer_resume(p->event_data);
    }
}

void el_trigger_disable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = false;
    if( p->event_type==EL_EVENT_TIMING){
        el_timer_pause(p->event_data);
    }
}

void el_delete_trigger(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(p);

    switch(p->event_type){

    case EL_EVENT_IR_RECEIVER_INCOMING:
        el_trg_exclusive_event_ir_receiver = NULL;
        break;

    case EL_EVENT_UART1_INCOMING:
        el_trg_exclusive_event_uart1 = NULL;
        break;

    case EL_EVENT_UART2_INCOMING:
        el_trg_exclusive_event_uart2 = NULL;
        break;

    case EL_EVENT_ACCELEROMETER_UPDATE:
        el_trg_exclusive_event_accelerometer = NULL;
        break;
        
    case EL_EVENT_IR_PROXIMITY_UPDATE:
        el_trg_exclusive_event_ir_proximity = NULL;
        break;

    case EL_EVENT_CAMERA_FRAME_UPDATE:
        el_trg_exclusive_event_camera_frame = NULL;
        break;

    case EL_EVENT_TIMING:
        el_delete_timer(p->event_data);
        break;

    }

    free(p);
}
