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
#include "el_ir_receiver.h"

el_bool el_irrc_enabled;
el_uint8 el_irrc_phase;
el_mcd el_irrc_timer;

el_uint16 el_irrc_counter;
el_uint8 el_irrc_check;
el_uint8 el_irrc_address;
el_uint8 el_irrc_data;

static union{
    el_uint16 FullBits;
    struct{
        el_uint16 CommandBits:6;
        el_uint16 AddressBits:5;
        el_uint16 ToggleBit:1;
        el_uint16 FieldBit:1;
        el_uint16 StartBit:1;
        el_uint16 PaddingBits:2;
    };
} el_irrc_packet;



void el_init_ir_receiver(){
    el_irrc_enabled = 0;
    el_irrc_phase = 0;
    el_irrc_timer = 0;
    el_irrc_counter = 0;
    el_irrc_check = 2;
    el_irrc_address = 0;
    el_irrc_data = 0;
}

void el_enable_ir_receiver(){
    el_irrc_phase = 0;
    el_irrc_timer = 0;
    el_irrc_check = 2;
    el_irrc_address = 0;
    el_irrc_data = 0;
    el_irrc_enabled = true;
}

void el_disable_ir_receiver(){
    el_irrc_enabled = false;
}

void el_ir_receiver_reset(){

    el_irrc_check = 2;
    el_irrc_address = 0;
    el_irrc_data = 0;
    el_irrc_phase = 0;
    el_irrc_timer = 0;
    
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;
    
}

/**

The ir receiver on e-puck is TSOP36236. It is supposed to be used with remote control 
based RC5 protocol. 64/36000 sec is the time interval between two data bits in the protocol. 

 **/
void el_routine_ir_receiver_14400hz(void){
    const el_mcd dk = EL_MASTERCLOCK_FREQ/14400;// should be 10 exactly
    const el_mcd period = EL_MASTERCLOCK_FREQ*(64/36000);// should be 256 exactly.
    int input;
    
    if(el_irrc_phase==0){
        return;
    }
    
    if(el_irrc_timer > 0){
        el_irrc_timer -= dk;
    }
    
    if(el_irrc_timer > 0){
        return;
    }else{
        el_irrc_timer += period;
    }
    
    if(el_irrc_phase){
        input = !REMOTE;// TSOP36236 gives inverted state (level = 1, burst = 0)
    }
    
    switch(el_irrc_phase){
    
    case 1:// start bit
        el_irrc_packet.FullBits = input;// put input on frist bit, also wipe all other bits
        el_irrc_phase++;
        break;
        
    case 2:// field bit
        if(input){
            // this bit must be low-high
            el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
            el_irrc_phase++;
        }else{
            // otherwise abort
            el_irrc_phase = 20;
            el_irrc_timer += 2*period;// wait for extra times
        }
        break;
        
    case 3:// toggle bit
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 4:// address bit 4
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 5:// address bit 3
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 6:// address bit 2
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 7:// address bit 1
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 8:// address bit 0
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 9:// command bit 5
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 10:// command bit 4
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 11:// command bit 3
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 12:// command bit 2
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 13:// command bit 1
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 14:// command bit 0
        el_irrc_packet.FullBits = (el_irrc_packet.FullBits<<1)|input;
        el_irrc_phase++;
        el_irrc_timer += period;
        break;
        
    case 15:// finishing
        el_irrc_phase = 0;
        el_irrc_timer = 0;
        /// output result
        el_irrc_data = el_irrc_packet.CommandBits;
        el_irrc_address = el_irrc_packet.AddressBits;
        el_irrc_check = el_irrc_packet.ToggleBit;
        /// automatic reset INT0
        //IFS0bits.INT0IF = 0;
        //IEC0bits.INT0IE = 1;
        el_irrc_counter++;
        /// signal a trigger event
        el_trg_event_flag_ex_irrc = 1;
        break;
        
    case 20:// abort
        el_irrc_phase = 0;
        el_irrc_timer = 0;
        /// reset INT0
        IFS0bits.INT0IF = 0;
        IEC0bits.INT0IE = 1;
        break;
        
    }
    
}

el_uint16 el_ir_receiver_get_counter(){
    return el_irrc_counter;
}

el_uint8 el_ir_receiver_get_check(){
    return el_irrc_check;
}

el_uint8 el_ir_receiver_get_address(){
    return el_irrc_address;
}

el_uint8 el_ir_receiver_get_data(){
    return el_irrc_data;
}
