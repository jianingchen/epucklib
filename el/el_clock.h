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

This is the header file of Masterclock. 

\author Jianing Chen

*/

#ifndef EL_CLOCK
#define EL_CLOCK

#include "el_common.h"

// general purpose time type, unit = 1/1000 second
typedef signed short el_time;
#define EL_TIME_FREQ   1000

// master clock time types, unit = 1/144000 second
typedef uint32_t el_mci;
#define EL_MASTERCLOCK_FREQ   (144000L)


el_mci el_get_masterclock();
el_time el_get_time_diff(el_mci k0,el_mci k1,el_mci*residue);



#ifdef EL_INCLUDE_CONTEXT

typedef int32_t el_mct;
typedef int16_t el_mcd;
#define EL_TIME_TO_MCT(t) ((el_mct)(EL_MASTERCLOCK_FREQ/EL_TIME_FREQ)*(t))

void el_init_masterclock();
void el_routine_masterclock_14400hz();

#endif



#endif
