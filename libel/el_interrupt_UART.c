
#include "el_context.h"
#include "el_uart.h"
#include "el_trigger.h"

void el_init_interrupt_UART(){

    // uart 1 receive
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
    IPC2bits.U1RXIP = 2;// UART1 interrupt priority

    // uart 1 transmit
    IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 0;
    IPC2bits.U1TXIP = 1;// UART1 interrupt priority

    // uart 2 receive
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 0;
    IPC6bits.U2RXIP = 1;// UART2 interrupt priority

    // uart 2 transmit
    IFS1bits.U2TXIF = 0;
    IEC1bits.U2TXIE = 0;
    IPC6bits.U2TXIP = 1;// UART2 interrupt priority

}

void __attribute__((interrupt, no_auto_psv))_U1RXInterrupt(void){
    char c;

    IFS0bits.U1RXIF = 0;

    c = U1RXREG;

    el_uart1_rx_buffer[el_uart1_rx_buf_i] = c;
    el_uart1_rx_buf_i++;
    el_uart1_rx_buf_i %= EL_UART_RX_BUF_DIM;
    if(el_uart1_rx_buf_i==el_uart1_rx_buf_o){
        el_uart1_rx_buf_o++;
        el_uart1_rx_buf_o %= EL_UART_RX_BUF_DIM;
    }

    if(c=='\r'){
        el_trg_event_flag_ex_uart1++;
    }

}

void __attribute__((interrupt, no_auto_psv))_U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0;
    
    if(el_uart1_tx_counter > 0){
        
        // transmit next char
        el_uart1_tx_pointer++;
        el_uart1_tx_counter--;

#ifdef EL_UART_NEWLINE_CRLF
        if(*el_uart1_tx_pointer=='\n'){
            /*
            note: dsPIC30F has a transmit buffer, so at most 4 chars
            can be fed into U1TXREG one by one. 
            */
            U1TXREG = '\r';
        }
#endif

        U1TXREG = *el_uart1_tx_pointer;

    }else{
        
        // transmission finished
        IEC0bits.U1TXIE = 0;

    }

}

#ifdef EL_USE_UART2
void __attribute__((interrupt, no_auto_psv))_U2RXInterrupt(void){

}

void __attribute__((interrupt, no_auto_psv))_U2TXInterrupt(void){

}
#endif
