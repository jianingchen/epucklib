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

#include "el_context.h"
#include "el_clock.h"

volatile el_mct el_masterclock;

void el_init_masterclock(){
    el_masterclock = 0;
}

void el_routine_masterclock_14400hz(){
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

void el_nop_delay(int x){
    int i;
    while(x--){
        i = 4996;
        while(i--) NOP();
    }
}
