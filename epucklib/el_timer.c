
#include "el_context.h"
#include "el_timer.h"

el_mct el_timers_mck;
el_mct el_timers_mcr;
el_list *el_timer_list;
volatile bool el_timer_list_lock;

void el_init_timers(){

    el_timer_list = el_create_list();

    el_timer_list_lock = false;

    el_timers_mck = el_get_masterclock();
    el_timers_mcr = 0;

}

el_handle el_create_timer(){
    el_timer *p;

    p = (el_timer*)malloc(sizeof(el_timer));

    p->period = 0;
    p->time = 0;
    p->rounds = 0;
    p->append_data = NULL;
    p->callback = NULL;
    p->paused = false;
    p->repeat = false;
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
    el_timer *p;
    el_time dt_ms;
    el_mct current_clock;

    // wait for on-going list operation
    while(el_timer_list_lock) NOP();
    el_timer_list_lock = true;

    // calculate time step
    current_clock = el_get_masterclock();
    dt_ms = el_get_time_diff(el_timers_mck,current_clock,&el_timers_mcr);
    el_timers_mck = current_clock - el_timers_mcr;
    el_timers_mcr = 0;

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
            
            if(p->time > 0){
                
                p->time -= dt_ms;
                
                if(p->time <= 0){
                    p->rounds++;
                    
                    if(p->callback){
                        p->callback(p->append_data);
                    }
                    
                }
                
            }

            if(p->time <= 0){
                if(p->repeat){
                    p->time += p->period;
                }else{
                    p->time = 0;
                }
            }
            
        }
        
    }
    el_list_loop_end(el_timer_list);
    
    el_timer_list_lock = false;

}

void el_timer_set_start(el_handle h,el_time time_ms){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);

    p->period = time_ms;
    p->time = time_ms;
    p->rounds = 0;
    p->paused = false;
    p->repeat = true;
    p->remove = false;

}

void el_timer_set_pause(el_handle h,el_time time_ms){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);

    p->period = time_ms;
    p->time = time_ms;
    p->rounds = 0;
    p->paused = true;
    p->repeat = true;
    p->remove = false;
    
}

void el_timer_set_callback(el_handle h,void (*f)(void*),void*a){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    
    p->append_data = a;
    p->callback = f;
    
}

void el_timer_pause(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = true;
}

void el_timer_resume(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    p->paused = false;
}

int el_timer_get_rounds(el_handle h){
    el_timer *p = (el_timer*)EL_HANDLE_TO_POINTER(h);
    return p->rounds;
}


