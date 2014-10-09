
#ifndef ELU_STDIO_H
#define	ELU_STDIO_H

#include "el_common.h"

#ifdef EL_INCLUDE_CONTEXT

#include "el_uart.h"
#include <stdio.h>
#include <stdarg.h>

#endif

/** improtant stdio functions for UART **/

#define ELU_STDIO_BUFFER_SIZE   96
#define ELU_STDIO_STACK_SIZE    384

EL_API int elu_snprintf(char*s,unsigned int n,const char*format,...) __attribute__ ((format(printf,3,4)));
EL_API int elu_sscanf(char*s,const char*format,...) __attribute__ ((format(scanf,2,3)));

EL_API void elu_set_stdio_target(int n);// UART channel
EL_API int elu_printf(const char*format,...) __attribute__ ((format(printf,1,2)));
EL_API int elu_scanf(const char*format,...) __attribute__ ((format(scanf,1,2)));

#endif	/* ELU_STDIO_H */
