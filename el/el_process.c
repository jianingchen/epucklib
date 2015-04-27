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
#include "el_process.h"
#include <setjmp.h>



#define EL_CMT_STATE_VACANT    0
#define EL_CMT_STATE_LAUNCH    1
#define EL_CMT_STATE_PAUSE     2
#define EL_CMT_STATE_RUNNING   3

typedef struct EL_CMTS{
    int state;
    el_mct wait_timer;
    void* arg_pointer;
    el_process function;
    jmp_buf function_context;
} el_cmts;


el_mct el_process_mck;
bool el_is_in_process;
jmp_buf el_cmt_main_buffer;
el_uint16 el_cmt_number_of_process;
el_cmts el_cmt_process_list[EL_PROCESS_DIM];
el_cmts *el_cmt_current_process;
el_index el_cmt_current_process_index;


static void el_cmt_warp_current_process(const int stack_offset){
    #ifdef _MSC_VER
    char *pad = (char*)_alloca(stack_offset);// for vc compiler
    #else
    char pad[stack_offset];// use dynamic amount of stack
    #endif

    pad[stack_offset - 1] = 0;// prevent it from getting optimized by compiler

    el_cmt_current_process->function(el_cmt_current_process->arg_pointer);
    
    el_cmt_current_process->state = EL_CMT_STATE_VACANT;
    el_cmt_current_process->function = NULL;
    el_cmt_current_process->arg_pointer = NULL;

    el_cmt_number_of_process--;
    longjmp(el_cmt_main_buffer,1);
}


void el_init_process(){
    int i;
    el_cmts *p;

    el_is_in_process = 0;

    for(i=0;i<EL_PROCESS_DIM;i++){
        p = el_cmt_process_list + i;
        p->state = EL_CMT_STATE_VACANT;
        p->wait_timer = EL_MCT_ZERO_POINT;
        p->arg_pointer = NULL;
        p->function = NULL;
    }
    el_cmt_current_process_index = -1;
    el_cmt_current_process = NULL;
    el_cmt_number_of_process = 0;

    el_process_mck = el_get_masterclock();

}

void el_routine_process(){
    el_mct current_clock;
    el_mct dk;
    el_cmts *p;
    int stack_offset;
    int i;

    // calculate time difference
    current_clock = el_get_masterclock();
    dk = current_clock - el_process_mck;
    el_process_mck = current_clock;
    
    // process the cooperative multi-task system
    el_is_in_process = true;

    for(i=0;i<EL_PROCESS_DIM;i++){

        el_cmt_current_process_index = i;
        p = el_cmt_process_list + i;

        if(p->wait_timer > EL_MCT_ZERO_POINT){
            continue;
        }

        if(p->state==EL_CMT_STATE_LAUNCH){

            if(!setjmp(el_cmt_main_buffer)){

                p->state = EL_CMT_STATE_RUNNING;
                el_cmt_number_of_process++;

                el_cmt_current_process = p;
                stack_offset = EL_PROCESS_STACK_OFFSET + i*EL_PROCESS_STACK_SIZE;
                el_cmt_warp_current_process(stack_offset);

            }

        }else
        if(p->state==EL_CMT_STATE_RUNNING){

            if(!setjmp(el_cmt_main_buffer)){
                el_cmt_current_process = p;
                longjmp(p->function_context,1);
            }

        }

    }
    
    el_cmt_current_process_index = -1;
    el_is_in_process = false;

    for(i=0;i<EL_PROCESS_DIM;i++){
        if(el_cmt_process_list[i].wait_timer > EL_MCT_ZERO_POINT){
            el_cmt_process_list[i].wait_timer -= dk;
        }
    }

}

void el_process_cooperate(){
    if(el_is_in_process){
        if(!setjmp(el_cmt_current_process->function_context)){
            longjmp(el_cmt_main_buffer,1);
        }
    }
}

void el_process_wait(el_time time_ms){
    if(el_is_in_process){
        el_cmt_current_process->wait_timer += EL_TIME_TO_MCT(time_ms);
        if(!setjmp(el_cmt_current_process->function_context)){
            longjmp(el_cmt_main_buffer,1);
        }
    }
}

void el_process_wait_fraction(unsigned int num,unsigned int den){
    if(el_is_in_process){
        el_cmt_current_process->wait_timer += (el_mct)num*EL_MASTERCLOCK_FREQ/(el_mct)den;
        if(!setjmp(el_cmt_current_process->function_context)){
            longjmp(el_cmt_main_buffer,1);
        }
    }
}

el_index el_launch_process(el_process func,void*arg){
    int i;
    el_cmts *p;

    if(func==NULL){
        return -1;
    }

    for(i=0;i<EL_PROCESS_DIM;i++){
        p = el_cmt_process_list + i;
        if(p->state==EL_CMT_STATE_VACANT){
            p->state = EL_CMT_STATE_LAUNCH;
            p->wait_timer = EL_MCT_ZERO_POINT;
            p->arg_pointer = arg;
            p->function = func;
            return i;
        }
    }

    return -1;
}

void el_process_resume(el_index i){
    el_cmts *p;
    p = el_cmt_process_list + i;
    p->wait_timer = EL_MCT_ZERO_POINT;
}

void el_kill_process(el_index i){
    el_cmts *p;
    if(el_cmt_current_process_index != i){
        p = el_cmt_process_list + i;
        p->state = EL_CMT_STATE_VACANT;
        p->wait_timer = EL_MCT_ZERO_POINT;
        p->arg_pointer = NULL;
        p->function = NULL;
    }
}

el_index el_get_local_process_index(){
    return el_cmt_current_process_index;
}
