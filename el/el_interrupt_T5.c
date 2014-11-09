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
#include "el_interrupt.h"
#include "el_camera.h"
#include "el_trigger.h"

void el_init_interrupt_T5(){

    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 1;//T5 Interrupt Enabled
    IPC5bits.T5IP = EL_INTP_T5;//T5 Interrupt Priority
    
    TMR5 = 0;
    PR5 = 1;
    T5CONbits.TCKPS = 0;//External Clock Prescsaler
    T5CONbits.TCS = 1;//External Clock
    T5CONbits.TON = 0;//Initially OFF
    
}

// frame interrput

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void){
    __asm__ volatile ("push     w0");
    __asm__ volatile ("bclr.b   _IFS1bits,#6");// IFS1bits.T5IF = 0;
    __asm__ volatile ("clr      _TMR5");
    __asm__ volatile ("mov      _el_cam_lock_buffer,w0");
    __asm__ volatile ("btst     w0,#0");
    __asm__ volatile ("bra      nz,DONT_SWAP");
__asm__ volatile ("SWAP_BUFFERS:");
    __asm__ volatile ("mov      _el_cam_w_frame,w0");
    __asm__ volatile ("push     _el_cam_r_frame");
    __asm__ volatile ("pop      _el_cam_w_frame");
    __asm__ volatile ("mov      w0,_el_cam_r_frame");// swapped
    __asm__ volatile ("inc      _el_cam_frame_counter");
    __asm__ volatile ("inc.b    _el_trg_event_flag_ex_cam");
    __asm__ volatile ("mov      _el_cam_w_frame,w0");
    __asm__ volatile ("add      w0,#4,w0");
    __asm__ volatile ("mov      w0,_el_cam_line_pointer");
    __asm__ volatile ("mov      w0,_el_cam_pixel_pointer");
    __asm__ volatile ("clr      _el_cam_y");
    __asm__ volatile ("bset.b	_T4CONbits+1,#7");// T4CONbits.TON = 1;
    __asm__ volatile ("pop      w0");
    __asm__ volatile ("retfie   ");
__asm__ volatile ("DONT_SWAP:");
    __asm__ volatile ("nop      ");
    __asm__ volatile ("nop      ");
    __asm__ volatile ("nop      ");
    __asm__ volatile ("nop      ");
    __asm__ volatile ("nop      ");
    __asm__ volatile ("mov      _el_cam_w_frame,w0");
    __asm__ volatile ("add      w0,#4,w0");
    __asm__ volatile ("mov      w0,_el_cam_line_pointer");
    __asm__ volatile ("mov      w0,_el_cam_pixel_pointer");
    __asm__ volatile ("clr      _el_cam_y");
    __asm__ volatile ("bset.b   _T4CONbits+1,#7");// T4CONbits.TON = 1;
    __asm__ volatile ("pop      w0");
}
