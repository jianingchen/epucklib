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

\defgroup ELU_STDIO UART STDIO

\section Introduction

(TODO)

*/
 
/*!

\file
\ingroup ELU_STDIO

\author Jianing Chen

*/

#ifndef ELU_STDIO_H
#define	ELU_STDIO_H

#include "el_common.h"
#include "el_uart.h"

#define _SNPRINTF_FAMILY   __attribute__ ((format(printf,3,4)))
#define _SSCANF_FAMILY   __attribute__ ((format(scanf,2,3)))

#define _PRINTF_FAMILY   __attribute__ ((format(printf,1,2)))
#define _SCANF_FAMILY   __attribute__ ((format(scanf,1,2)))


/*!
    \brief snprintf to be used here

    \return length of the string
    
*/
int elu_snprintf(char*s,unsigned int n,const char*format,...) _SNPRINTF_FAMILY;


/*!
    \brief sscanf to be used here

    \return number of data field successfuly scanned

*/
int elu_sscanf(const char*s,const char*format,...) _SSCANF_FAMILY;


/*!
    \brief send a string with data fields to UART1

    \return length of the string with data formatted

*/
int elu_printf(const char*format,...) _PRINTF_FAMILY;


/*!
    \brief send a string with data fields + a newline char to UART1

    \return length of the string with data formatted

*/
int elu_println(const char*format,...) _PRINTF_FAMILY;


int elu_scanf(const char*format,...) _SCANF_FAMILY;


int elu_putchar(int c);


void elu_scanf_set_echo(bool k);

#endif	/* ELU_STDIO_H */
