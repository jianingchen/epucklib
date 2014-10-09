
#include "el_context.h"
#include "elu_stdio.h"

int8_t elu_stdio_target;
char eluart_stdio_buffer[ELU_STDIO_BUFFER_SIZE];

void elu_set_stdio_target(int n){
    elu_stdio_target = n;
}

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
    int i;
    char *buffer = eluart_stdio_buffer;
    int offset;

    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    
    length = elu_warp_vsnprintf(offset,buffer,ELU_STDIO_BUFFER_SIZE,format,args);
    
    va_end(args);

    for(i=0;i<length;i++){
        if(buffer[i]=='\n'){
            el_uart_putch(EL_UART_1,'\r');
        }
        el_uart_putch(EL_UART_1,buffer[i]);
    }

    return length;
}

int elu_scanf(const char *format,...){
    va_list args;
    int length;
    int entries;
    char *buffer = eluart_stdio_buffer;
    char c;
    int offset;
    
    length = 0;
    do{
        c = el_uart_getch(EL_UART_1);

        if(c=='\r'){
            buffer[length] = '\0';
            break;
        }else{
            buffer[length] = c;
            length++;
        }

    }while(length<ELU_STDIO_BUFFER_SIZE);
    
    va_start(args,format);
    
    offset = EL_EPUCK_MEMORY_SIZE - ELU_STDIO_STACK_SIZE - (int)&offset;
    
    entries = elu_warp_vsscanf(offset,buffer,format,args);

    va_end(args);

    return entries;
}
