
/*!

\defgroup EL_BASIC_CONCEPTS Basic Concepts


\section sec_el_handle About 'el_handle' (handle of an internal object)

In C and C++, an object typically means a struct or a class. 
A 'handle' is similar to a pointer to an object, which are generally used
to refer to some object. However, a handle does not include the information
about structure of the object. Thus, a handle can only be used to refer to
an object but not access the data inside the object. All operations towards
such type of object need to be achieved by its related functions. 


\section sec_el_index About 'el_index' (component index)

A variable of this type is typically used to refer to a hardware component. 
Regardless of the physical arrangement in the circuit, the indices of 
the components in the same module is made to be in serial and always start 
from 0, for example, 0 for left stepper motor and 1 for right stepper motor. 
Typically, macros are defined to assign a readable tag to the indices, 
for example, "EL_LED_RING_2" and "EL_LED_RING_3". 

'el_index' is a signed type in which a negative value typically has specific
meaning. For example, for the led module, -1 ("EL_LED_RING_ALL") means 
all leds on the ring of the e-puck. 


\section sec_el_flags About 'el_flags' (binary option list)

This is the type used to pass a group of binary option as one variable.
This is often used to provide a multiple choice when each of the choice
can only be YES or NO. 


\section sec_el_mci About Masterclock

Masterclock of the system is an incremental timer that keeps 
increasing after the system is initialized. In each real time second, 
Masterclock will be increased by 144000. Masterclock is frequenctly 
used to calculate the interval between two time points. 

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
typedef signed char     el_int8;
typedef signed int      el_int16;
typedef signed long     el_int32;
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

#endif	/* EL_COMMON_H */

