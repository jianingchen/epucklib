/**

Jianing Chen
j.n.chen@sheffield.ac.uk

==== Minimum Cooperative Multitasking Library ====

See bottom part for a debug/demo code. 

**/

#include "cmt.h"

typedef struct CMTS{
    int state;
    cmt_timer wait_timer;
    void* data_pointer;
    cmt_function function;
    jmp_buf function_context;
} cmts;

// cmt system context
unsigned int CMT_MAX_N_PROCESS;
unsigned int CMT_PROCESS_STACK_SIZE;
unsigned int CMT_MAIN_STACK_RESERVATION_SIZE;
jmp_buf cmt_main_buffer;
int cmt_number_of_process;
cmts *cmt_process_list;
cmts *cmt_current_process;
int cmt_current_process_index;

void cmt_initialize(const cmt_ini*setting){
    int i;
    cmts *p;
    
    if(setting){
        CMT_MAX_N_PROCESS = setting->MaxNumberOfProcess;
        CMT_PROCESS_STACK_SIZE = setting->ProcessStackSzie;
        CMT_MAIN_STACK_RESERVATION_SIZE = setting->MainStackReservation;
    }else{
        CMT_MAX_N_PROCESS = 32;
        CMT_PROCESS_STACK_SIZE = 8192;
        CMT_MAIN_STACK_RESERVATION_SIZE = 32768;
    }
    
    cmt_process_list = (cmts*)malloc(sizeof(cmts)*CMT_MAX_N_PROCESS);
    
    for(i=0;i<CMT_MAX_N_PROCESS;i++){
        p = cmt_process_list + i;
        p->state = CMT_STATE_VACANT;
        p->wait_timer = 0;
        p->data_pointer = NULL;
        p->function = NULL;
    }
    cmt_current_process_index = -1;
    cmt_current_process = NULL;
    cmt_number_of_process = 0;
}

int cmt_launch_process_delay(cmt_function f,void*f_data,cmt_time time){
    int i;
    cmts *p;
    
    if(f==NULL){
        return -1;
    }
    
    for(i=0;i<CMT_MAX_N_PROCESS;i++){
        p = cmt_process_list + i;
        if(p->state==CMT_STATE_VACANT){
            p->state = CMT_STATE_LAUNCH;
            p->wait_timer = time;
            p->data_pointer = f_data;
            p->function = f;
            return i;
        }
    }
    
    return -1;
}

int cmt_launch_process(cmt_function f,void*f_data){
    return cmt_launch_process_delay(f,f_data,0);
}

void cmt_cooperate(){
    if(!setjmp(cmt_current_process->function_context)){
        longjmp(cmt_main_buffer,1);
    }
}

void cmt_wait(cmt_time time){
    cmt_current_process->wait_timer += time;
    cmt_cooperate();
}

static void cmt_warp_current_process(const int stack_offset){
    char pad[stack_offset];
    
    pad[stack_offset - 1] = 0;// prevent it from getting optimized-out by compiler
    
    cmt_current_process->function(cmt_current_process->data_pointer);
    
    cmt_current_process->state = CMT_STATE_VACANT;
    cmt_current_process->function = NULL;
    cmt_current_process->data_pointer = NULL;
    cmt_number_of_process--;
    longjmp(cmt_main_buffer,1);
}

void cmt_main_routine(){
    int i;
    int stack_offset;
    cmts *p;
    
    for(i=0;i<CMT_MAX_N_PROCESS;i++){
        
        cmt_current_process_index = i;
        p = cmt_process_list + i;
        
        if(p->wait_timer>0){
            continue;
        }
        
        if(p->state==CMT_STATE_LAUNCH){
            
            if(!setjmp(cmt_main_buffer)){
                
                p->state = CMT_STATE_RUNNING;
                cmt_number_of_process++;
                
                cmt_current_process = p;
                stack_offset = CMT_MAIN_STACK_RESERVATION_SIZE + CMT_PROCESS_STACK_SIZE*i;
                cmt_warp_current_process(stack_offset);
                
            }
            
        }else
        if(p->state==CMT_STATE_RUNNING){
            
            if(!setjmp(cmt_main_buffer)){
                cmt_current_process = p;
                longjmp(p->function_context,1);
            }
            
        }
        
    }
    
    cmt_current_process_index = -1;
}

void cmt_process_timers(cmt_time time_elapsed){
    int i;
    for(i=0;i<CMT_MAX_N_PROCESS;i++){
        if(cmt_process_list[i].wait_timer > 0){
            cmt_process_list[i].wait_timer -= time_elapsed;
        }
    }
}

int cmt_get_current_process_index(){
    return cmt_current_process_index;
}

int cmt_get_number_of_process(){
    return cmt_number_of_process;
}

void cmt_terminate(void){
    if(cmt_process_list){
        free(cmt_process_list);
        cmt_process_list = NULL;
    }
}





//==============================================================================
/*

debug build section

build with: 
gcc -g cmt.c -DCMT_DEBUG_MAIN

*/
#ifdef CMT_DEBUG_MAIN

#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int stack_overflow(){
    int waste[8000];
    waste[7999] = 0;
    return waste[7999];
}

void routineA(void*data){
    int i;
    
    printf("<A: %p>",&i);
    
    for(i=0;i<10;i++){
        
        printf("[A: %d]",i);
        
        //stack_overflow();
        
        cmt_wait(500);
        
    }
    
    printf("[A: end]");
    
}


void routineB(void*data){
    int i;
    
    printf("<B: %p>",&i);
    
    for(i=0;i<30;i++){
        
        printf("[B: %d]",i);
        
        cmt_wait(300);
        
    }
    
    printf("[B: end]");
    
}

void routineD(void*data){
    int i;
    
    printf("<D: %p>",&i);
    
    for(i=0;i<10;i++){
        
        printf("[D: %d]",i);
        
        if(i==4){
            cmt_launch_process(routineA,NULL);
        }
        
        
        cmt_wait(1000);
        
    }
    
    printf("[D: end]");
    
}

void routineC(void*data){
    int i;
    
    printf("<C: %p>",&i);
    
    for(i=0;i<100;i++){
        
        printf("[C: %d]",i);
        
        if(i==89){
            cmt_launch_process(routineD,NULL);
        }
        
        cmt_wait(100);
        
    }
    
    printf("[C: end]");
    
}

void routineW(void*data){
    int i;
    
    printf("<W: %p>",&i);
    
    while(1){
        
        if(data){
            printf("[%s]",(char*)data);
        }
        
        cmt_cooperate();
    }
    
    printf("[W: end]");
    
}


void routineX(void*data){
    int i;
    
    printf("<X: %p>",&i);
    
    i = 0;
    while(1){
        
        printf("[X: %d]",i++);
        
        cmt_wait(100);
        
    }
    
    printf("[X: end]");
    
}
// "i" in X and Y should be increased in a ratio of 3:2
void routineY(void*data){
    int i;
    
    printf("<Y: %p>",&i);
    
    i = 0;
    while(1){
        
        printf("[Y: %d]",i++);
        
        cmt_wait(150);
        
    }
    
    printf("[Y: end]");
    
}

void routineZ(void*data){
    int i;
    
    printf("<Z: %p>",&i);
    
    printf("[Z: end]");
    
}

int main(int argc,char*argv[]){
    const unsigned int STEP_SIZE_MS = 100;
    char strA[] = "Hello";
    char strB[] = "World";
    int i;
    cmt_ini typical_setting;
    
    typical_setting.MaxNumberOfProcess = 32;
    typical_setting.ProcessStackSzie = 2048;
    typical_setting.MainStackReservation = 65536;
    
    cmt_initialize(&typical_setting);
    
    printf("\nCMT Initialized");
    printf("\n%d",sizeof(jmp_buf));
    
    cmt_launch_process(routineW,strA);
    cmt_launch_process(routineW,strB);
    
    cmt_launch_process(routineX,NULL);
    cmt_launch_process(routineY,NULL);
    cmt_launch_process_delay(routineZ,NULL,5000);
    
    cmt_launch_process(routineA,NULL);
    cmt_launch_process(routineB,NULL);
    cmt_launch_process(routineC,NULL);
    
    i = 0;
    while(i<500){
        
        printf("\n%5.5d\t%6.3lf\t",i,(double)i*STEP_SIZE_MS/1000.0);
        
        cmt_main_routine();
        cmt_process_timers(STEP_SIZE_MS);
        
        i++;
        
        Sleep(STEP_SIZE_MS);
    }
    
    cmt_terminate();
    
    printf("\nCMT Terminated");
    
    if(cmt_get_number_of_process()){
        printf("\nCMT Warning: %d process leaked !!!",cmt_get_number_of_process());
    }
    
    printf("\n");
    
    system("pause");
    
    return 0;
}

#endif
