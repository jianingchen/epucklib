
#ifndef EL_UART_H
#define	EL_UART_H

#include "el_common.h"

#ifdef EL_INCLUDE_CONTEXT

void el_init_uart();

#endif

#define EL_UART_1       1
#define EL_UART_2       2
#define EL_UART_BLUETOOTH   EL_UART_1

EL_API void el_uart_putch(int channel,char c);
EL_API char el_uart_getch(int channel);
EL_API bool el_uart_has_char(int channel);

EL_API void el_uart_receive_string(int channel,char*buffer,unsigned int buffer_size);
EL_API void el_uart_send_string(int channel,const char*str);
EL_API void el_uart_send_dec_int(int channel,int d);// send the integer as a string of it's decimal form
EL_API void el_uart_send_dec_uint(int channel,unsigned int d);
EL_API void el_uart_send_hex_uint(int channel,unsigned int h);


#endif	/* EL_UART_H */
