
#include "el_context.h"
#include "el_timer.h"

el_mct el_timer_mck;
el_list *el_timer_list;
volatile bool el_timer_list_lock;

void el_init_timers(){

    el_timer_list = el_create_list();
    el_timer_list_lock = false;
    
    el_timer_mck = el_get_masterclock();

}

el_handle el_create_timer(){
    el_timer *p;

    p = (el_timer*)malloc(sizeof(el_timer));

    p->period = 0;
    p->count_down = 0;
    p->rounds = 0;
    p->append_data = NULL;
    p->callback = NULL;
    p->paused = false;
    p->repeat = true;
    p->remove = false;

    while(el_timer_list_lock) NOP();
    el_timer_list_lock = true;

    el_list_append_item(el_timer_list,p);

    el_timer_list_lock = false;

    return EL_POINTER_TO_HANDLE(p);
}

void el_delete_timer(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->remove = true;// mark for delete in next iteration
}

void el_routine_timers(){
    el_mct current_clock;
    el_ct dk;
    el_timer *p;

    // wait for on-going list operation
    while(el_timer_list_lock) NOP();
    el_timer_list_lock = true;

    // calculate time difference
    current_clock = el_get_masterclock();
    dk = current_clock - el_timer_mck;
    el_timer_mck = current_clock;
    
    // loop through all timers
    el_list_loop_begin(el_timer_list);
    while(el_list_loop_has_next(el_timer_list)){
        el_list_loop_step(el_timer_list);
        
        p = (el_timer*)el_list_loop_get_item(el_timer_list);

        if(p->remove){
            
            el_list_loop_remove_item(el_timer_list);
            free(p);
            
        }else
        if(!p->paused){
            
            if(p->count_down > 0){
                
                p->count_down -= dk;
                
                if(p->count_down <= 0){
                    p->rounds++;
                    
                    if(p->callback){
                        p->callback(p->append_data);
                    }
                    
                }
                
            }

            if(p->count_down <= 0){
                if(p->repeat){
                    p->count_down += p->period;
                }else{
                    p->count_down = 0;
                }
            }
            
        }
        
    }
    el_list_loop_end(el_timer_list);
    
    el_timer_list_lock = false;

}

void el_timer_start(el_handle h,el_time time_ms){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    el_mct k = EL_TIME_TO_MTK(time_ms);
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

void el_timer_set_callback(el_handle h,void (*f)(void*),void*a){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->append_data = a;
    p->callback = f;
}

void el_timer_set_perodic(el_handle h,bool perodic){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->repeat = perodic;
}

void el_timer_pause(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = true;
}

void el_timer_resume(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = false;
}

uint16_t el_timer_get_rounds(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    return p->rounds;
}

void el_timer_set_rounds(el_handle h,uint16_t n){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->rounds = n;
}
