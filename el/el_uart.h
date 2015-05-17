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

/*!

\defgroup EL_UART UART Serial Communication

\section Introduction

The functions in this module are the basic functions to use the UART bus 
on the microcontroller. In \ref ELU_STDIO, a more user-friendly set of functions 
are provided to transmit and receive text. 

*/
 
/*!

\file
\ingroup EL_UART

\author Jianing Chen

*/

#ifndef EL_UART_H
#define	EL_UART_H

#include "el_common.h"

#define EL_UART_1       0
#define EL_UART_2       1
#define EL_UART_BLUETOOTH   EL_UART_1


/*!
    \brief Setup a value that: upon receving that value, reset the chip. 

    \param k        whether to use such a feature or not
    \param x        the value that triggers a hardware reset
    
    The Tiny Bootloader will first send a specific value to the UART bus shortly 
    before a microcontroller programming. After that, the microcontroller needs 
    to be reseted in a short time window. This reset code feature let the 
    microcontroller reset itself automatically once the value occurs. The image 
    below shows the setting in the TinyBootloader when the code is 128 (used 
    in the examples of this lib). 
    \image html images/doc_tinybld_setup.png
*/
void el_uart_use_reset_code(el_bool k,el_int8 x);


/*!
    \brief Send one byte of data to the serial channel.

    \param ch       serial channel index (at present, only 0 is effective)
    \param x        the value to be sent

    Note: the return of this function does not necessarily mean the transmission is finished.
    To determine whether the transmission is still undergoing, use ::el_uart_is_sending.
*/
void el_uart_send_char(el_index ch,char c);


/*!
    \brief Send a string (an array of char terminated by a null) to the serial channel.

    \param ch       serial channel index (at present, only 0 is effective)
    \param str      pointer to the string to be sent

    Note: the return of this function does not necessarily mean the transmission is finished.
    To determine whether the transmission is still undergoing, use ::el_uart_is_sending.
*/
void el_uart_send_string(el_index ch,const char*str);


/*!
    \brief Check whether the transmission issued in the last sending function is finished.

    \param ch       serial channel index (at present, only 0 is effective)

    \return         Boolean value of "is still sending or not"
    
*/
bool el_uart_is_sending(el_index ch);


/*!
    \brief Check how many bytes are received and buffered in the channel.

    \param ch       serial channel index (at present, only 0 is effective)

    \return         the number of bytes received and buffered

*/
el_uint16 el_uart_get_char_counter(el_index ch);


/*!
    \brief Take out one byte from the receiving buffer in order of first-in-first-out.

    \param ch       serial channel index (at present, only 0 is effective)

    \return         the byte

*/
char el_uart_get_char(el_index ch);


/*!
    \brief Totally clear up the receiving buffer of a serial channel.

    \param ch       serial channel index (at present, only 0 is effective)

*/
void el_uart_flush_char(el_index ch);


/*!
    \brief Get a string ended with a null (0 or '\0') or a carriage-return ('\\r', (the "Enter" key). 

    \param ch           serial channel index (at present, only 0 is effective)
    \param buf          pointer of a buffer to store the received string
    \param buf_size     size of the given buffer, which gives a limit to the length of the string received
    \param return_line  finish the string once (1) a carriage-return, (0) a null is received.

    \return         length of the string

    The null char received at last is not notable as a string in C is always
    terminated by a null. 
    In case of ending with a carriage-return, the carriage-return itself will 
    be placed in the string and followed by a null that terminates the string 
    as usual. 
*/
el_uint16 el_uart_get_string(el_index ch,char*buf,el_uint16 buf_size,el_bool return_line);


/*!
    \brief Check the first byte in the receiving buffer, but does not remove it from the buffer.

    \param ch       serial channel index (at present, only 0 is effective)

    \return         value of the first byte

*/
char el_uart_peek_first_char(el_index ch);


/*!
    \brief Check the last byte in the receiving buffer but do not remove it from the buffer.

    \param ch       serial channel index (at present, only 0 is effective)

    \return         value of the last byte

*/
char el_uart_peek_last_char(el_index ch);


/*!
    \brief Remove the last byte received in the receiving buffer.

    \param ch       serial channel index (at present, only 0 is effective)

*/
void el_uart_erase_last_char(el_index ch);


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

#define EL_UART_NEWLINE_CRLF

#define EL_UART_BAUDRATE    115200
#define EL_UART_TX_BUF_DIM  80
#define EL_UART_RX_BUF_DIM  64

extern volatile el_bool el_uart_reset_code_enable;
extern el_uint8 el_uart_reset_code;
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
