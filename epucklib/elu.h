
#ifndef ELU_H
#define ELU_H

#include "el.h"

/** improtant stdio functions for UART **/

#define ELU_STDIO_BUFFER_SIZE   96
#define ELU_STDIO_STACK_SIZE    384

int elu_snprintf(char*s,unsigned int n,const char*format,...) __attribute__ ((format(printf,3,4)));
int elu_sscanf(char*s,const char*format,...)__attribute__ ((format(scanf,2,3)));

void elu_set_stdio_target(int n);
int elu_printf(const char*format,...) __attribute__ ((format(printf,1,2)));
int elu_scanf(const char*format,...)__attribute__ ((format(scanf,1,2)));

#endif
