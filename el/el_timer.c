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
#include "el_timer.h"

el_mci el_timer_mck;
bool el_is_in_timer_callback;
el_uint8 el_timer_reg_i;
el_timer*el_timer_reg[EL_TIMER_DIM];
el_timer*el_calling_timer;

void el_init_timers(){
    int i;

    el_timer_reg_i = 0;
    for(i=0;i<EL_TIMER_DIM;i++){
        el_timer_reg[i] = NULL;
    }
    
    el_is_in_timer_callback = false;
    el_timer_mck = el_get_masterclock();

}

static void el_timer_step(el_timer*p,el_mct dk){

    if(p->paused){
        return;
    }
    
    // countdown
    if(p->count_down > 0){
        p->count_down -= dk;
    }

    // timer expired
    if(p->count_down <= 0){
        
        p->rounds++;
        
        if(p->repeat){
            p->count_down += p->period;
        }else{
            p->count_down = 0;
            p->paused = true;
        }
        
        if(p->callback){
            el_is_in_timer_callback = true;
            el_calling_timer = p;
            p->callback(p->append_data);
            el_calling_timer = NULL;
            el_is_in_timer_callback = false;
        }
        
    }

}

el_handle el_timer_callback_get_handle(){
    return EL_POINTER_TO_HANDLE(el_calling_timer);
}

void el_routine_timers(){
    el_mci current_clock;
    el_mct dk;
    el_timer *p;
    int i,d;
    
    /** calculate time difference **/
    current_clock = el_get_masterclock();
    dk = current_clock - el_timer_mck;
    el_timer_mck = current_clock;
    
    /** deal with all timers **/
    d = el_timer_reg_i;// note: el_timer_reg_i may be modified in timer callback
    for(i=0;i<d;i++){
        el_timer_step(el_timer_reg[i],dk);
    }
    
    /** register maintenance (remove&squeeze) **/
    d = 0;
    for(i=0;i<el_timer_reg_i;i++){
        p = el_timer_reg[i];
        if(p->remove){
            free(p);
        }else{
            el_timer_reg[d] = p;
            d++;
        }
    }
    el_timer_reg_i = d;
    
}

el_handle el_create_timer(){
    el_timer *p;
    
    if(el_timer_reg_i>=EL_TIMER_DIM){
        return NULL;
    }
    
    p = (el_timer*)malloc(sizeof(el_timer));

    p->period = 0;
    p->count_down = 0;
    p->rounds = 0;
    p->append_data = NULL;
    p->callback = NULL;
    p->paused = false;
    p->repeat = true;
    p->remove = false;

    el_timer_reg[el_timer_reg_i] = p;
    el_timer_reg_i++;

    return EL_POINTER_TO_HANDLE(p);
}

void el_delete_timer(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->remove = true;
}

void el_timer_start(el_handle h,el_time time_ms){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    el_mct k = EL_TIME_TO_MCT(time_ms);
    p->period = k;
    p->count_down = k;
    p->rounds = 0;
    p->paused = false;
    p->remove = false;
}

void el_timer_start_fraction(el_handle h,int num,int den){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    el_mct k = (el_mct)num*EL_MASTERCLOCK_FREQ/(el_mct)den;
    p->period = k;
    p->count_down = k;
    p->rounds = 0;
    p->paused = false;
    p->remove = false;
}

void el_timer_set_callback(el_handle h,el_timer_callback f,void*arg){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->append_data = arg;
    p->callback = f;
}

void el_timer_set_perodic(el_handle h,bool is_perodic){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->repeat = is_perodic;
}

void el_timer_pause(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = true;
}

void el_timer_resume(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = false;
}

el_uint32 el_timer_get_rounds(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    return p->rounds;
}

void el_timer_set_rounds(el_handle h,el_uint32 n){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->rounds = n;
}
