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
#include "elu_stdio.h"

#include <stdio.h>
#include <stdarg.h>

#define ELU_STDIO_UART          EL_UART_1
#define ELU_STDIO_STACK_SIZE    400

char elu_stdio_buffer[ELU_STDIO_BUFFER_SIZE];
bool elu_stdio_scanf_echo = true;

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

int elu_sscanf(const char*s,const char*format,...){
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

        while(el_uart_is_sending(ELU_STDIO_UART)){
            el_process_cooperate();
        }
        
        el_uart_send_string(ELU_STDIO_UART,elu_stdio_buffer);

    }

    return length;
}

int elu_println(const char *format,...){
    va_list args;
    int length;
    int offset;

    va_start(args,format);

    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;

    length = elu_warp_vsnprintf(offset,elu_stdio_buffer,ELU_STDIO_BUFFER_SIZE - 1,format,args);

    elu_stdio_buffer[length++] = '\n';
    elu_stdio_buffer[length] = '\0';

    va_end(args);

    if(length > 0){

        while(el_uart_is_sending(ELU_STDIO_UART)){
            el_process_cooperate();
        }

        el_uart_send_string(ELU_STDIO_UART,elu_stdio_buffer);

    }

    return length;
}

int elu_scanf(const char *format,...){
    va_list args;
    int length;
    int entries;
    int offset;
    char c;
    
    length = 0;

    do{

        // wait for a incoming char
        while(el_uart_get_char_counter(ELU_STDIO_UART)<=length){
            el_process_cooperate();
        }
        
        // check the incoming char
        c = el_uart_peek_last_char(ELU_STDIO_UART);

        // apply backsapce
        if(c=='\b'){
            el_uart_erase_last_char(ELU_STDIO_UART);
            el_uart_erase_last_char(ELU_STDIO_UART);
        }
        
        length = el_uart_get_char_counter(ELU_STDIO_UART);
        
        if(elu_stdio_scanf_echo){
            while(el_uart_is_sending(ELU_STDIO_UART)){
                el_process_cooperate();
            }
            if(c=='\r'){
                el_uart_send_char(ELU_STDIO_UART,'\n');
            }else{
                el_uart_send_char(ELU_STDIO_UART,c);
            }
        }

        //proceed to scan when the key pressed is ENTER

    }while(c!='\r');

    length = el_uart_get_string(ELU_STDIO_UART,elu_stdio_buffer,ELU_STDIO_BUFFER_SIZE);

    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    
    entries = elu_warp_vsscanf(offset,elu_stdio_buffer,format,args);

    va_end(args);

    elu_stdio_buffer[0] = '\0';

    return entries;
}

void elu_scanf_set_echo(bool k){
    elu_stdio_scanf_echo = k;
}

int elu_putchar(int c){
    while(el_uart_is_sending(ELU_STDIO_UART)){
        el_process_cooperate();
    }
    el_uart_send_char(ELU_STDIO_UART,c);
    return c;
}
