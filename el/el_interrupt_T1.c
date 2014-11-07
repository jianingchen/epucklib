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

void el_init_interrupt_T1(){
    
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;//T1 Interrupt Enabled
    IPC0bits.T1IP = EL_INTP_T1;//T1 Interrupt Priority

    TMR1 = 0;
    PR1 = 1;
    T1CONbits.TCKPS = 1;//External Clock Prescsaler
    T1CONbits.TSYNC = 0;//Syncronised External Clock
    T1CONbits.TCS = 1;//External Clock
    T1CONbits.TON = 0;//Initially OFF

}

// pixel interrupt

void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void){
    __asm__ volatile ("push.s   ");// save original data in w0,w1,w2,w3
    __asm__ volatile ("mov      _PORTD,w2");// get the pixel's high byte from CAM_DATA
    __asm__ volatile ("bclr.b   _IFS0bits,#3");// IFS0bits.T1IF = 0;
    __asm__ volatile ("mov      _PORTD,w3");// get the pixel's low byte from CAM_DATA
    __asm__ volatile ("mov.b	_PORTCbits,WREG");
    __asm__ volatile ("btst	w0,#3");//  check bit3 of PORTC (CAM_HREF) 
    __asm__ volatile ("bra	z,HREF_LOW");
    __asm__ volatile ("");
    __asm__ volatile ("mov	_el_cam_pixel_pointer,w0");
    __asm__ volatile ("mov      #0xFF00,w1");// prepare the mask of the high byte (first 8 bits)
    __asm__ volatile ("and      w1,w2,w2");// masking for the high byte (keep only first 8 bits)
    __asm__ volatile ("lsr      w3,#8,w3");// shift the low byte to the low 8 bits (keep only last 8 bits)
    __asm__ volatile ("ior      w2,w3,[w0++]");// merge the high byte and the low byte of the pixel
    __asm__ volatile ("mov	w0,_el_cam_pixel_pointer");
    __asm__ volatile ("pop.s    ");// restore w0,w1,w2,w3
    __asm__ volatile ("retfie");
__asm__ volatile ("HREF_LOW:");
    __asm__ volatile ("bclr.b	_T1CONbits+1,#7");// T1CONbits.TON = 0;
    __asm__ volatile ("mov	_el_cam_pixel_pointer,w2");
    __asm__ volatile ("mov	_el_cam_line_pointer,w3");
    __asm__ volatile ("mov	#80,w0");
    __asm__ volatile ("add	w0,w3,w0");
    __asm__ volatile ("mov	w0,_el_cam_pixel_pointer");
    __asm__ volatile ("mov	w0,_el_cam_line_pointer");
    __asm__ volatile ("sub	w2,w3,w2");
    __asm__ volatile ("asr	w2,w2");// shift right (divided by 2)
    __asm__ volatile ("mov      w2,_el_cam_x");
    __asm__ volatile ("inc      _el_cam_y");
    __asm__ volatile ("mov      _el_cam_y,w3");
    __asm__ volatile ("mov      _el_cam_w_frame,w0");
    __asm__ volatile ("mov      w2,[w0++]");
    __asm__ volatile ("mov      w3,[w0]");
    __asm__ volatile ("pop.s    ");// restore w0,w1,w2,w3
}


/*
    __asm__ volatile ("push.s   ");// save original data in w0,w1,w2,w3
    __asm__ volatile ("");
    __asm__ volatile ("mov      _PORTD,w2");// get the pixel's high byte from CAM_DATA
    __asm__ volatile ("mov	_PORTCbits,w0");// get PORTC
    __asm__ volatile ("mov      _PORTD,w3");// get the pixel's low byte from CAM_DATA
    __asm__ volatile ("and	w0,#8,w0");// mask PORTC to get CAM_HREF
    __asm__ volatile ("sub      w0,#0,w0");// check if CAM_HREF is zero
    __asm__ volatile ("bra      z,LINE_FINISH");// branch check if last op is zero
__asm__ volatile ("READ_PIXEL:");
    __asm__ volatile ("mov      #0xFF00,w0");// prepare the mask of the high byte (first 8 bits)
    __asm__ volatile ("and      w0,w2,w2");// masking for the high byte (keep only first 8 bits)
    __asm__ volatile ("lsr      w3,#8,w3");// shift the low byte to the low 8 bits (keep only last 8 bits)
    __asm__ volatile ("ior      w2,w3,w3");// merge the high byte and the low byte of the pixel
    __asm__ volatile ("mov      _el_cam_pixel_pointer,w0");
    __asm__ volatile ("mov      w3,[w0]");
    __asm__ volatile ("inc2     w0,w0");
    __asm__ volatile ("mov      w0,_el_cam_pixel_pointer");
    __asm__ volatile ("bra      END");
__asm__ volatile ("LINE_FINISH:");
    __asm__ volatile ("bclr.b	_T1CONbits+1,#7");// T1CONbits.TON = 0;
__asm__ volatile ("END:");
    __asm__ volatile ("bclr.b   _IFS0bits,#3");// IFS0bits.T1IF = 0;
    __asm__ volatile ("pop.s    ");// restore w0,w1,w2,w3
*/