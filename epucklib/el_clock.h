
/*!

\file
\ingroup EL_BASIC_CONCEPTS

This is the header file of Masterclock. 

\author Jianing Chen

*/

#ifndef EL_CLOCK
#define EL_CLOCK

#include "el_common.h"

typedef signed short el_time;// general purpose time type, unit = 1 millisecond
#define EL_TIME_FREQ   1000



#ifdef EL_INCLUDE_CONTEXT

// clock time types, unit = 1/144000 second. for internal use only.
typedef uint32_t el_mci;
typedef int32_t el_mct;
typedef int16_t el_mcd;
#define EL_MASTERCLOCK_FREQ   (144000L)
#define EL_TIME_TO_MCT(t) ((el_mct)144*(t))

void el_init_masterclock();
void el_routine_masterclock_14400hz();

el_mci el_get_masterclock();

el_time el_get_time_diff(el_mci c0,el_mci c1,el_mct*residue);

#endif



#endif
