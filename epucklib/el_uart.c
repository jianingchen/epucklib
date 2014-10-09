
#include "el_context.h"
#include "el_uart.h"

void el_init_uart(){
    
}

void el_uart_putch(int channel,char c){
    switch(channel){
        
    case EL_UART_1:
        while(e_uart1_sending()) NOP();
        e_send_uart1_char(&c,1);
        break;
        
    case EL_UART_2:
        while(e_uart2_sending()) NOP();
        e_send_uart2_char(&c,1);
        break;
    }
}

char el_uart_getch(int channel){
    char c;
    
    switch(channel){
        
    case EL_UART_1:
        while(e_getchar_uart1(&c)==0) NOP();
        break;
        
    case EL_UART_2:
        while(e_getchar_uart2(&c)==0) NOP();
        break;
    }
    
    return c;
}
