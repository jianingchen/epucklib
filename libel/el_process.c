
#include "el_context.h"
#include "el_process.h"
#include "cmt.h"

el_mci el_process_mck;
bool el_is_in_process;

void el_init_process(){
    cmt_ini cmt_setup;

    cmt_setup.MaxNumberOfProcess = EL_PROCESS_DIM;
    cmt_setup.ProcessStackSzie = EL_PROCESS_STACK_SIZE;
    cmt_setup.ProcessStackOffset = EL_PROCESS_STACK_OFFSET;
    cmt_initialize(&cmt_setup);
    
    el_is_in_process = 0;
    
    el_process_mck = el_get_masterclock();

}

void el_routine_process(){
    el_mci current_clock;
    el_mct dk;

    // calculate time difference
    current_clock = el_get_masterclock();
    dk = current_clock - el_process_mck;
    el_process_mck = current_clock;
    
    // process the cooperative multi-task system
    el_is_in_process = true;
    cmt_main_routine();
    cmt_process_timers(dk);
    el_is_in_process = false;

}

void el_process_wait(el_time time_ms){
    if(el_is_in_process){
        cmt_wait(EL_TIME_TO_MCT(time_ms));
    }
}

void el_process_wait_fraction(unsigned int num,unsigned int den){
    if(el_is_in_process){
        cmt_wait((el_mct)num*EL_MASTERCLOCK_FREQ/(el_mct)den);
    }
}

void el_process_cooperate(){
    if(el_is_in_process){
        cmt_cooperate();
    }
}

int el_launch_process(el_process function,void*appended_data){
    return cmt_launch_process_delay(function,appended_data,0);
}
