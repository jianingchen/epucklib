
#include "el_context.h"
#include "el_uart.h"

el_uint16 el_uart1_tx_counter;
char*el_uart1_tx_pointer;
char el_uart1_tx_buffer[EL_UART_TX_BUF_DIM];

volatile el_uint8 el_uart1_rx_buf_i;
volatile el_uint8 el_uart1_rx_buf_o;
char el_uart1_rx_buffer[EL_UART_RX_BUF_DIM];

#ifdef EL_USE_UART2
el_uint16 el_uart2_tx_counter;
char*el_uart2_tx_pointer;
char el_uart2_tx_buffer[EL_UART_TX_BUF_DIM];

volatile el_uint8 el_uart2_rx_buf_i;
volatile el_uint8 el_uart2_rx_buf_o;
char el_uart2_rx_buffer[EL_UART_RX_BUF_DIM];
#endif

void el_init_uart(){
    int i;

    /** UART 1 **/

    el_uart1_tx_counter = 0;
    el_uart1_tx_pointer = el_uart1_tx_buffer;
    for(i=0;i<EL_UART_TX_BUF_DIM;i++){
        el_uart1_tx_buffer[i] = '\0';
    }

    el_uart1_rx_buf_i = 0;
    el_uart1_rx_buf_o = 0;
    for(i=0;i<EL_UART_RX_BUF_DIM;i++){
        el_uart1_rx_buffer[i] = '\0';
    }

    U1MODE = 0;
    U1STA = 0;
    
    U1BRG = (EL_EPUCK_FCY/EL_UART_BAUDRATE)/16 - 1;
    U1STAbits.UTXISEL = 1;// data, parity and stop bits
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;// enable transmit


    /** UART 2 **/

    U2MODE = 0;
    U2STA = 0;
    
    // see "el_interrupt_UART.c" for UART RX/TX interrupt related code
    
}

bool el_uart_is_sending(el_index channel){

    switch(channel){

    case EL_UART_1:
        return el_uart1_tx_counter > 0;

    case EL_UART_2:
        return false;

    }

    return false;
}

void el_uart1_tx_clear(){
    IEC0bits.U1TXIE = 0;
    el_uart1_tx_counter = 0;
    el_uart1_tx_pointer = el_uart1_tx_buffer;
}

void el_uart1_tx_issue(){
    IEC0bits.U1TXIE = 1;
    el_uart1_tx_counter--;
#ifdef EL_UART_NEWLINE_CRLF
        if(*el_uart1_tx_pointer=='\n'){
            U1TXREG = '\r';
        }
#endif
    U1TXREG = *el_uart1_tx_pointer;
}

void el_uart2_tx_clear(){

}

void el_uart2_tx_issue(){

}

void el_uart_send_string(el_index channel,const char*str){
    int i;
    char c;
    
    switch(channel){

    case EL_UART_1:
        el_uart1_tx_clear();
        i = 0;
        while(i < EL_UART_TX_BUF_DIM){
            c = str[i];
            if(c=='\0'){
                break;
            }
            el_uart1_tx_buffer[i] = c;
            i++;
        }
        if(i > 0){
            el_uart1_tx_pointer = el_uart1_tx_buffer;
            el_uart1_tx_counter = i;
            el_uart1_tx_issue();
        }
        break;

    case EL_UART_2:

        break;
    }
}

void el_uart_send_char(el_index channel,char c){
    switch(channel){
        
    case EL_UART_1:
        el_uart1_tx_clear();
        el_uart1_tx_buffer[0] = c;
        el_uart1_tx_counter = 1;
        el_uart1_tx_pointer = el_uart1_tx_buffer;
        el_uart1_tx_issue();
        break;
        
    case EL_UART_2:

        break;
    }
}

el_uint16 el_uart_get_char_counter(el_index channel){
    el_uint16 i,o;
    
    switch(channel){

    case EL_UART_1:
        o = el_uart1_rx_buf_o;
        i = el_uart1_rx_buf_i;
        if(i >= o){
            return i - o;
        }else{
            return EL_UART_RX_BUF_DIM + i - o;
        }
        
    case EL_UART_2:
        return 0;

    }

    return 0;
}

char el_uart_get_char(el_index channel){
    char c = 0xFF;
    
    switch(channel){
        
    case EL_UART_1:
        if(el_uart1_rx_buf_i != el_uart1_rx_buf_o){
            c = el_uart1_rx_buffer[el_uart1_rx_buf_o];
            el_uart1_rx_buf_o++;
            el_uart1_rx_buf_o %= EL_UART_RX_BUF_DIM;
        }
        break;
        
    case EL_UART_2:
        break;
        
    }
    
    return c;
}

el_uint16 el_uart_get_string(el_index channel,char*buf,unsigned int l){
    char c = 0xFF;
    int length;

    length = 0;
    l--;

    switch(channel){

    case EL_UART_1:
        while(el_uart1_rx_buf_i != el_uart1_rx_buf_o){
            c = el_uart1_rx_buffer[el_uart1_rx_buf_o];
            el_uart1_rx_buf_o++;
            el_uart1_rx_buf_o %= EL_UART_RX_BUF_DIM;
            if(c=='\0'){
                break;
            }
            buf[length] = c;
            length++;
            if(length>=l){
                break;
            }
        }
        buf[length] = '\0';
        break;

    case EL_UART_2:
        break;

    }

    return length;
}

char el_uart_peek_first_char(el_index channel){
    int i,o;

    switch(channel){

    case EL_UART_1:
        i = el_uart1_rx_buf_i;
        o = el_uart1_rx_buf_o;
        if(i != o){
            return el_uart1_rx_buffer[o];
        }
        break;

    case EL_UART_2:
        break;

    }

    return 0xFF;
}

char el_uart_peek_last_char(el_index channel){
    int i,o;

    switch(channel){

    case EL_UART_1:
        i = el_uart1_rx_buf_i;
        o = el_uart1_rx_buf_o;
        if(i != o){
            i--;
            i %= EL_UART_RX_BUF_DIM;
            return el_uart1_rx_buffer[i];
        }
        break;

    case EL_UART_2:
        break;

    }

    return 0xFF;
}

void el_uart_flush_char(el_index channel){

    switch(channel){

    case EL_UART_1:
        el_uart1_rx_buf_i = 0;
        el_uart1_rx_buf_o = 0;
        break;

    case EL_UART_2:
        break;

    }

}
