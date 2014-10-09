
#ifndef EL_COMMON_H
#define	EL_COMMON_H

#include <stdlib.h>
#include <stdint.h>

#define EL_API
#define BIT(b) (1<<(b))

typedef int el_enum;
typedef void* el_handle;
typedef unsigned char bool;
#define false 0
#define true 1

#define el_nop() {__asm__ volatile ("nop");}

#endif	/* EL_COMMON_H */

