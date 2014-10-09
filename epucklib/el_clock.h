
#ifndef EL_CLOCK
#define EL_CLOCK

#include "el_common.h"

typedef signed short el_time;// general purpose time type, unit = 1 millisecond
#define EL_TIME_FREQ   1000



#ifdef EL_INCLUDE_CONTEXT

// clock time types, unit = 1/144000 second. for internal use only.
typedef signed short el_ct;
typedef signed long el_mct;
#define EL_MASTERCLOCK_FREQ   (144000L)
#define EL_TIME_TO_MTK(t) ((el_mct)144*(t))

extern volatile el_mct el_masterclock;

void el_init_masterclock();
void el_routine_masterclock();

el_mct el_get_masterclock();
el_time el_get_time_diff(el_mct c0,el_mct c1,el_mct*residue);
el_time el_time_sec(float second);

#endif



#endif
