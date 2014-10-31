
#include "el_context.h"
#include "el_trigger.h"

bool el_is_in_trigger_condition = 0;
el_uint8 el_trigger_reg_i = 0;
el_trigger*el_trigger_reg[EL_TRIGGER_DIM] = {0};

volatile uint8_t el_trg_event_flag_ex_uart1 = 0;
volatile uint8_t el_trg_event_flag_ex_uart2 = 0;
volatile uint8_t el_trg_event_flag_ex_irrc = 0;
volatile uint8_t el_trg_event_flag_ex_cam = 0;
volatile uint8_t el_trg_event_flag_ex_acc = 0;
volatile uint8_t el_trg_event_flag_ex_irps = 0;

void el_init_triggers(){
    int i;
    
    el_is_in_trigger_condition = false;

    el_trigger_reg_i = 0;
    for(i=0;i<EL_TRIGGER_DIM;i++){
        el_trigger_reg[i] = NULL;
    }
    
    el_trg_event_flag_ex_uart1 = 0;
    el_trg_event_flag_ex_uart2 = 0;
    el_trg_event_flag_ex_irrc = 0;
    el_trg_event_flag_ex_cam = 0;
    el_trg_event_flag_ex_acc = 0;
    el_trg_event_flag_ex_irps = 0;
    
}

static void el_trg_proceed(el_trigger *trg){
    bool K;
    
    if(trg->enabled){
        
        if(trg->condition){
            el_is_in_trigger_condition = true;
            K = trg->condition(trg);
            el_is_in_trigger_condition = false;
        }else{
            K = true;
        }
        
        if(K){
            trg->counter++;
            if(trg->process){
                trg->enabled = false;
                el_launch_process(trg->process,trg);
            }
        }
        
    }
    
}

void el_routine_triggers(){
    el_trigger *p;
    int i,d;
    
    /** deal with different events **/
    
    d = el_trigger_reg_i;
    
    if(el_trg_event_flag_ex_uart1){
        el_trg_event_flag_ex_uart1 = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_UART1_ENTERED){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    if(el_trg_event_flag_ex_uart2){
        el_trg_event_flag_ex_uart2 = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_UART2_ENTERED){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    if(el_trg_event_flag_ex_irrc){
        el_trg_event_flag_ex_irrc = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_IR_RECEIVER_INCOME){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    if(el_trg_event_flag_ex_acc){
        el_trg_event_flag_ex_acc = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_ACCELEROMETER_UPDATE){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    if(el_trg_event_flag_ex_irps){
        el_trg_event_flag_ex_irps = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_IR_PROXIMITY_UPDATE){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    if(el_trg_event_flag_ex_cam){
        el_trg_event_flag_ex_cam = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_CAMERA_FRAME_UPDATE){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }
    
    /** register maintenance (remove&squeeze) **/
    
    d = 0;
    for(i=0;i<el_trigger_reg_i;i++){
        p = el_trigger_reg[i];
        if(p->remove){
            free(p);
        }else{
            el_trigger_reg[d] = p;
            d++;
        }
    }
    el_trigger_reg_i = d;
    
}

el_handle el_create_trigger(){
    el_trigger *p;
    
    if(el_trigger_reg_i>=EL_TRIGGER_DIM){
        return NULL;
    }
    
    p = (el_trigger*)malloc(sizeof(el_trigger));
    
    p->counter = 0;
    p->event_type = 0;
    p->event_data = NULL;
    p->condition = NULL;
    p->process = NULL;
    p->enabled = true;
    p->remove = false;
    
    el_trigger_reg[el_trigger_reg_i] = p;
    el_trigger_reg_i++;
    
    return EL_POINTER_TO_HANDLE(p);
}

void el_delete_trigger(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(p);
    p->remove = true;
}

void el_trigger_set_event(el_handle trigger,el_enum event_type){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->event_type = event_type;
    p->event_data = NULL;
}

void el_trigger_set_condition(el_handle trigger,el_condition function){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->condition = function;
}

void el_trigger_set_process(el_handle trigger,el_process function){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->process = function;
}

void el_trigger_enable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = true;
}

void el_trigger_disable(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->enabled = false;
}

uint16_t el_trigger_get_counter(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    return p->counter;
}
