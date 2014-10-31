
#include "el_context.h"
#include "el_process.h"
#include "elu_stdio.h"

#include <stdio.h>
#include <stdarg.h>

#define ELU_STDIO_STACK_SIZE    384
#define ELU_STDIO_BUFFER_SIZE   80

const el_index elu_stdio_target = EL_UART_1;
char elu_stdio_buffer[ELU_STDIO_BUFFER_SIZE];

int elu_warp_vsnprintf(const int offset,char*s,size_t n,const char*format,va_list arg){
    unsigned char warp_space[offset];
    warp_space[offset-1] = 0;
    return vsnprintf(s,n,format,arg);
}

int elu_warp_vsscanf(const int offset,const char*s,const char*format,va_list arg){
    unsigned char warp_space[offset];
    warp_space[offset-1] = 0;
    return vsscanf(s,format,arg);
}

int elu_snprintf(char*s,unsigned int n,const char*format,...){
    va_list args;
    int r;
    int offset;
    
    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    r = elu_warp_vsnprintf(offset,s,n,format,args);
    
    va_end(args);
    
    return r;
}

int elu_sscanf(char*s,const char*format,...){
    va_list args;
    int r;
    int offset;
    
    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    r = elu_warp_vsscanf(offset,s,format,args);
    
    va_end(args);
    
    return r;
}

int elu_printf(const char *format,...){
    va_list args;
    int length;
    int offset;
    
    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    
    length = elu_warp_vsnprintf(offset,elu_stdio_buffer,ELU_STDIO_BUFFER_SIZE,format,args);
    
    va_end(args);

    if(length > 0){

        while(el_uart_is_sending(elu_stdio_target)){
            el_process_cooperate();
        }
        
        el_uart_send_string(elu_stdio_target,elu_stdio_buffer);

    }

    return length;
}

int elu_scanf(const char *format,...){
    va_list args;
    int length;
    int entries;
    int offset;

    while(el_uart_get_char_counter(elu_stdio_target)==0){
        el_process_cooperate();
    }

    length = el_uart_get_string(elu_stdio_target,elu_stdio_buffer,ELU_STDIO_BUFFER_SIZE);

    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    
    entries = elu_warp_vsscanf(offset,elu_stdio_buffer,format,args);

    va_end(args);

    return entries;
}
