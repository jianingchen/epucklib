
#ifndef ELU_STDIO_H
#define	ELU_STDIO_H

#include "el_common.h"
#include "el_uart.h"

int elu_snprintf(char*s,unsigned int n,const char*format,...) __attribute__ ((format(printf,3,4)));
int elu_sscanf(char*s,const char*format,...) __attribute__ ((format(scanf,2,3)));

int elu_printf(const char*format,...) __attribute__ ((format(printf,1,2)));
int elu_scanf(const char*format,...) __attribute__ ((format(scanf,1,2)));
void elu_scanf_set_echo(bool k);

#endif	/* ELU_STDIO_H */
