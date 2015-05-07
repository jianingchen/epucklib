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

#include "el_context.h"
#include "el_trigger.h"

el_bool el_is_in_trigger_condition;
el_uint16 el_trigger_overwatch;
el_uint8 el_trigger_reg_i;
el_trigger*el_trigger_reg[EL_TRIGGER_DIM];

volatile el_uint8 el_trg_event_flag_in[6];
volatile el_uint8 el_trg_event_flag_ex_uart1;
volatile el_uint8 el_trg_event_flag_ex_uart2;
volatile el_uint8 el_trg_event_flag_ex_irrc;
volatile el_uint8 el_trg_event_flag_ex_cam;
volatile el_uint8 el_trg_event_flag_ex_acc;
volatile el_uint8 el_trg_event_flag_ex_irps;

void el_init_triggers(){
    int i;

    el_is_in_trigger_condition = false;
    el_trigger_overwatch = 0;

    el_trigger_reg_i = 0;
    for(i=0;i<EL_TRIGGER_DIM;i++){
        el_trigger_reg[i] = NULL;
    }

    for(i=0;i<6;i++){
        el_trg_event_flag_in[i] = 0;
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
            el_trigger_overwatch = 0;
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
    int i,j,d;

    d = el_trigger_reg_i;
    el_trigger_overwatch = 0;

    /** deal with different events **/

    // for all internal events
    for(j=0;j<6;j++){
        if(el_trg_event_flag_in[j]){
            el_trg_event_flag_in[j] = 0;
            // for all triggers
            for(i=0;i<d;i++){
                if(el_trigger_reg[i]->event_type==(EL_EVENT_INTERNAL_A + j)){
                    el_trg_proceed(el_trigger_reg[i]);
                }
            }
        }
    }

    if(el_trg_event_flag_ex_uart1){
        el_trg_event_flag_ex_uart1 = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_UART1_RECEIVED){
                el_trg_proceed(el_trigger_reg[i]);
            }
        }
    }

    if(el_trg_event_flag_ex_uart2){
        el_trg_event_flag_ex_uart2 = 0;
        for(i=0;i<d;i++){
            if(el_trigger_reg[i]->event_type==EL_EVENT_UART2_RECEIVED){
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
        el_error_signal = EL_SYS_ERROR_TRIGGER_CREATE_FAIL;
        return NULL;
    }

    p = (el_trigger*)malloc(sizeof(el_trigger));
    if(p==NULL){
        el_error_signal = EL_SYS_ERROR_MALLOC_NULL;
        return NULL;
    }

    p->counter = 0;
    p->event_type = 0;
    p->event_data = NULL;
    p->condition = NULL;
    p->process = NULL;
    p->enabled = true;
    p->remove = false;

    el_trigger_reg[el_trigger_reg_i++] = p;

    return EL_POINTER_TO_HANDLE(p);
}

void el_delete_trigger(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(p);
    p->remove = true;
}

void el_trigger_set_event(el_handle trigger,el_trigger_event_type e){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    p->event_type = e;
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

el_bool el_trigger_is_enabled(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    return p->enabled;
}

el_uint32 el_trigger_get_counter(el_handle trigger){
    el_trigger *p = EL_HANDLE_TO_POINTER(trigger);
    return p->counter;
}

void el_trigger_issue_internal_event(el_enum e){
    if((EL_EVENT_INTERNAL_A <= e)&&(e <= EL_EVENT_INTERNAL_F)){
        el_trg_event_flag_in[e - EL_EVENT_INTERNAL_A] = 1;
    }
}
