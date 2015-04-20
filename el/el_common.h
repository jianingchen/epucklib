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

\file
\ingroup EL_BASIC_CONCEPTS

This header file defines 'el_enum', 'el_flags' and 'el_handle'. 

\author Jianing Chen

*/

#ifndef EL_COMMON_H
#define	EL_COMMON_H

#include <stdlib.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1
#define false 0
#define true 1
typedef unsigned char   bool;
typedef char            el_int8;
typedef int             el_int16;
typedef long            el_int32;
typedef unsigned char   el_uint8;
typedef unsigned int    el_uint16;
typedef unsigned long   el_uint32;
typedef void*           el_handle;
typedef int             el_index;
typedef bool            el_bool;
typedef unsigned int    el_flags;
typedef unsigned int    el_enum;

#define BIT(b) (1<<(b))
#define el_nop() {__asm__ volatile ("nop");}

#define NEVER_RETURN   void __attribute__ ((noreturn))

#endif	/* EL_COMMON_H */

