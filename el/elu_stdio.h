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

\section uartstdio_sec_1 Introduction

This module let the user interface UART1 in a similar fashion as the stdio 
functions. For example, sending formated string using "printf" alike functions 
and recieving string using "scanf" alike functions. 

Compared to the relatively native interfacing functions in \ref EL_UART, 
the functions provide more convience. However, it is only OK to use them in 
one process. In other word, one should avoid call these functions concurently 
in more than one process in the system. 


\section uartstdio_sec_2 Important Notice

The "sprintf" and "sscanf" (and other formatted string functions) in the C 
standard library are unlikely to work in a process here due to the stack size 
they required. In this library, "elu_sscanf" and "elu_snprintf" are provided 
to achieve same functionalities. 

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

    \param format   same rule as the format string of "printf"
    \return length of the string with data formatted

*/
int elu_printf(const char*format,...) _PRINTF_FAMILY;


/*!
    \brief send a string with data fields + a newline char to UART1

    \param format   same rule as the format string of "printf"
    \return length of the string with data formatted

*/
int elu_println(const char*format,...) _PRINTF_FAMILY;


/*!
    \brief scanf to be used here
    
    \param format   same rule as the format string of "scanf"
    \return number of data field successfuly scanned
    
    This function behaves similarly to the "scanf": it receives and stores
    characters. When a carriage return (the Enter key) occurs, this function 
    analyzes the string and returns. 
*/
int elu_scanf(const char*format,...) _SCANF_FAMILY;


/*!
    \brief configure whether ::elu_scanf echo the character received 
    
    \param k   echo or not
*/
void elu_scanf_set_echo(bool k);


int elu_putchar(int c);


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

#define ELU_STDIO_BUFFER_SIZE   80
extern char elu_stdio_buffer[ELU_STDIO_BUFFER_SIZE];

#endif

#endif	/* ELU_STDIO_H */
