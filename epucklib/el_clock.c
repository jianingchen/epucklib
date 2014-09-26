
#include "el_context.h"
#include "el_clock.h"

volatile el_mct el_masterclock;

void el_init_masterclock(){
    el_masterclock = 0;
}

void el_routine_masterclock(){
    el_masterclock += 10;
}

el_mct el_get_masterclock(){
    return el_masterclock;
}

el_time el_get_time_diff(el_mct c0,el_mct c1,el_mct*residue){
    const el_mct ratio = EL_MASTERCLOCK_FREQ/EL_TIME_FREQ;
    el_mct d;
    el_mct r;
    el_time dt;

    if(c0<c1){

        d = c1 - c0;

        r = d%ratio;
        d /= ratio;

        dt = d;

    }else{

        d = c0 - c1;

        r = d%ratio;
        d /= ratio;
        
        dt = -d;

    }

    if(residue){
        *residue = r;
    }
    
    return dt;
}

