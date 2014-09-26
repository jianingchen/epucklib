
#include "el_context.h"
#include "el_trigger.h"

#define EL_PROCESS_USE_MASTERCLOCK  1

bool el_is_in_cmt_process;
el_mct el_triggers_mck;
el_mct el_triggers_mcr;

el_trigger*el_triggers_ir_proximity_event;
el_trigger*el_triggers_ir_receiver_event;
el_trigger*el_triggers_ir_camera_event;
el_trigger*el_triggers_ir_uart1_event;
el_trigger*el_triggers_ir_uart2_event;

void el_init_triggers(){
    cmt_ini cmt_setup;

    cmt_setup.MainStackReservation = 128;
    cmt_setup.MaxNumberOfProcess = 8;
    cmt_setup.ProcessStackSzie = 160;
    cmt_initialize(&cmt_setup);
    
    el_is_in_cmt_process = false;

    el_triggers_mck = el_get_masterclock();
    el_triggers_mcr = 0;

}

#if EL_PROCESS_USE_MASTERCLOCK

void el_routine_triggers(){
    el_mct current_clock;
    el_ct dk;

    // calculate time step (use master clock)
    current_clock = el_get_masterclock();
    dk = current_clock - el_triggers_mck;
    el_triggers_mck = current_clock;

    // process the cooperative multi-task system
    el_is_in_cmt_process = true;

    cmt_main_routine();
    cmt_process_timers(dk);

    el_is_in_cmt_process = false;

}

void el_process_wait(el_time time_ms){
    if(el_is_in_cmt_process){
        cmt_wait((EL_MASTERCLOCK_FREQ/1000)*time_ms);
    }
}

#else

void el_routine_triggers(){
    el_mct current_clock;
    el_time dt_ms;

    // calculate time step (use general time)
    current_clock = el_get_masterclock();
    dt_ms = el_get_time_diff(el_triggers_mck,current_clock,&el_triggers_mcr);
    el_triggers_mck = current_clock - el_triggers_mcr;
    el_triggers_mcr = 0;

    // process the cooperative multi-task system
    el_is_in_cmt_process = true;

    cmt_main_routine();
    cmt_process_timers(dt_ms);

    el_is_in_cmt_process = false;

}

void el_process_wait(el_time time_ms){
    if(el_is_in_cmt_process){
        cmt_wait(time_ms);
    }
}

#endif

void el_process_cooperate(){
    if(el_is_in_cmt_process){
        cmt_cooperate();
    }
}

int el_launch_process(el_process function,void*appended_data){
    return cmt_launch_process_delay(function,appended_data,0);
}
