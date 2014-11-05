
/*!

\defgroup EL_UART UART Serial Communication

\section Introduction

(TODO)

*/
 
/*!

\file
\ingroup EL_UART

\author Jianing Chen

*/

#ifndef EL_UART_H
#define	EL_UART_H

#include "el_common.h"

#define EL_UART_1       1
#define EL_UART_2       2
#define EL_UART_BLUETOOTH   EL_UART_1

void el_uart_send_char(el_index ch,char c);
void el_uart_send_string(el_index ch,const char*str);
bool el_uart_is_sending(el_index ch);

el_uint16 el_uart_get_char_counter(el_index ch);
char el_uart_get_char(el_index ch);
void el_uart_flush_char(el_index ch);
el_uint16 el_uart_get_string(el_index ch,char*buf,unsigned int size);
char el_uart_peek_first_char(el_index ch);
char el_uart_peek_last_char(el_index ch);
void el_uart_erase_last_char(el_index ch);



#ifdef EL_INCLUDE_CONTEXT

#define EL_UART_NEWLINE_CRLF

#define EL_UART_BAUDRATE    115200
#define EL_UART_TX_BUF_DIM  80
#define EL_UART_RX_BUF_DIM  64

extern el_uint16 el_uart1_tx_counter;
extern char*el_uart1_tx_pointer;
extern char el_uart1_tx_buffer[EL_UART_TX_BUF_DIM];
extern volatile el_uint8 el_uart1_rx_buf_i;
extern volatile el_uint8 el_uart1_rx_buf_o;
extern char el_uart1_rx_buffer[EL_UART_RX_BUF_DIM];

void el_init_uart();
void el_uart1_tx_clear();
void el_uart1_tx_issue();
void el_uart2_tx_clear();
void el_uart2_tx_issue();

#endif

#endif	/* EL_UART_H */
