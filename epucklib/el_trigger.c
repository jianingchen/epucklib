
#include "el_context.h"
#include "el_trigger.h"

el_mci el_trigger_mck;
bool el_is_in_condition;

volatile uint8_t el_trg_event_flag_ex_uart1;
volatile uint8_t el_trg_event_flag_ex_uart2;
volatile uint8_t el_trg_event_flag_ex_irrc;
volatile uint8_t el_trg_event_flag_ex_cam;
volatile uint8_t el_trg_event_flag_ex_acc;
volatile uint8_t el_trg_event_flag_ex_irps;

el_trigger*el_trg_event_reg_ex_uart1;
el_trigger*el_trg_event_reg_ex_uart2;
el_trigger*el_trg_event_reg_ex_irrc;
el_trigger*el_trg_event_reg_ex_cam;
el_trigger*el_trg_event_reg_ex_acc;
el_trigger*el_trg_event_reg_ex_irps;

void el_init_triggers(){
    
    el_is_in_condition = false;

    el_trg_event_flag_ex_uart1 = 0;
    el_trg_event_flag_ex_uart2 = 0;
    el_trg_event_flag_ex_irrc = 0;
    el_trg_event_flag_ex_cam = 0;
    el_trg_event_flag_ex_acc = 0;
    el_trg_event_flag_ex_irps = 0;

    el_trg_event_reg_ex_uart1 = NULL;
    el_trg_event_reg_ex_uart2 = NULL;
    el_trg_event_reg_ex_irrc = NULL;
    el_trg_event_reg_ex_cam = NULL;
    el_trg_event_reg_ex_acc = NULL;
    el_trg_event_reg_ex_irps = NULL;
    
    el_trigger_mck = el_get_masterclock();

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

void el_trigger_register_event(el_handle trigger,el_enum event_type){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    
    switch(event_type){
        
    case EL_EVENT_UART1_INCOMING:
        el_trg_event_reg_ex_uart1 = p;
        p->event_type = event_type;
        break;
        
    case EL_EVENT_UART2_INCOMING:
        el_trg_event_reg_ex_uart2 = p;
        p->event_type = event_type;
        break;

    case EL_EVENT_IR_RECEIVER_INCOMING:
        el_trg_event_reg_ex_irrc = p;
        p->event_type = event_type;
        break;
        
    case EL_EVENT_ACCELEROMETER_UPDATE:
        el_trg_event_reg_ex_acc = p;
        p->event_type = event_type;
        break;

    case EL_EVENT_IR_PROXIMITY_UPDATE:
        el_trg_event_reg_ex_irps = p;
        p->event_type = event_type;
        break;

    case EL_EVENT_CAMERA_FRAME_UPDATE:
        el_trg_event_reg_ex_cam = p;
        p->event_type = event_type;
        break;
        
    default:
        p->event_type = 0;
        break;
    }
    
}

void el_delete_trigger(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(p);
    
    switch(p->event_type){

    case EL_EVENT_UART1_INCOMING:
        el_trg_event_reg_ex_uart1 = NULL;
        break;

    case EL_EVENT_UART2_INCOMING:
        el_trg_event_reg_ex_uart2 = NULL;
        break;
        
    case EL_EVENT_IR_RECEIVER_INCOMING:
        el_trg_event_reg_ex_irrc = NULL;
        break;
        
    case EL_EVENT_ACCELEROMETER_UPDATE:
        el_trg_event_reg_ex_acc = NULL;
        break;
        
    case EL_EVENT_IR_PROXIMITY_UPDATE:
        el_trg_event_reg_ex_irps = NULL;
        break;
        
    case EL_EVENT_CAMERA_FRAME_UPDATE:
        el_trg_event_reg_ex_cam = NULL;
        break;
        
    }

    free(p);
}

void el_trigger_enable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = true;
}

void el_trigger_disable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = false;
}

static void el_trg_proceed(el_trigger *trg){
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
                trg->enabled = false;
                el_launch_process(trg->process,trg);
            }
        }
    }
}

void el_routine_triggers(){
    el_mci current_clock;
    el_mct dk;
    
    /** calculate time difference **/
    
    current_clock = el_get_masterclock();
    dk = current_clock - el_trigger_mck;
    el_trigger_mck = current_clock;
    
    
    
    /** deal with events **/
    
    if(el_trg_event_flag_ex_uart1){
        el_trg_event_flag_ex_uart1 = 0;
        if(el_trg_event_reg_ex_uart1){
            el_trg_proceed(el_trg_event_reg_ex_uart1);
        }
    }
    
    if(el_trg_event_flag_ex_uart2){
        el_trg_event_flag_ex_uart2 = 0;
        if(el_trg_event_reg_ex_uart2){
            el_trg_proceed(el_trg_event_reg_ex_uart2);
        }
    }

    if(el_trg_event_flag_ex_irrc){
        el_trg_event_flag_ex_irrc = 0;
        if(el_trg_event_reg_ex_irrc){
            el_trg_proceed(el_trg_event_reg_ex_irrc);
        }
    }
    
    if(el_trg_event_flag_ex_cam){
        el_trg_event_flag_ex_cam = 0;
        if(el_trg_event_reg_ex_cam){
            el_trg_proceed(el_trg_event_reg_ex_cam);
        }
    }
    
    if(el_trg_event_flag_ex_acc){
        el_trg_event_flag_ex_acc = 0;
        if(el_trg_event_reg_ex_acc){
            el_trg_proceed(el_trg_event_reg_ex_acc);
        }
    }
    
    if(el_trg_event_flag_ex_irps){
        el_trg_event_flag_ex_irps = 0;
        if(el_trg_event_reg_ex_irps){
            el_trg_proceed(el_trg_event_reg_ex_irps);
        }
    }
    
}
