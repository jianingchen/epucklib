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

void el_init_interrupt_T4(){

    IFS1bits.T4IF = 0;
    IEC1bits.T4IE = 1;//T4 Interrupt Enabled
    IPC5bits.T4IP = EL_INTP_T4;//T4 Interrupt Priority

    TMR4 = 0;
    PR4 = 1;
    T4CONbits.TCKPS = 1;//External Clock Prescsaler
    T4CONbits.TCS = 1;//External Clock
    T4CONbits.TON = 0;//Initially OFF

}

// line interrupt

void __attribute__((interrupt, no_auto_psv))_T4Interrupt(void){
    __asm__ volatile ("push.d   w0");// save original data in w0,w1
    __asm__ volatile ("mov.b	_PORTCbits,WREG");
    __asm__ volatile ("btst	w0,#4");// check bit4 of PORTC (CAM_VSYNC)
    __asm__ volatile ("bra	nz,VSYNC_HIGH");
__asm__ volatile ("VSYNC_LOW:");
    __asm__ volatile ("bclr.b	_IFS1bits,#5");//IFS1bits.T4IF = 0;
    __asm__ volatile ("clr	_TMR4");
    __asm__ volatile ("bclr.b	_T4CONbits+1,#7");// T4CONbits.TON = 0
    __asm__ volatile ("pop.d    w0");// restore w0,w1
    __asm__ volatile ("retfie");
__asm__ volatile ("VSYNC_HIGH:");
    __asm__ volatile ("mov	_PORTD,w0");//<<R5G3
    __asm__ volatile ("push     w2");// save original data in w2
    __asm__ volatile ("mov	_PORTD,w1");//<<G3B5
    __asm__ volatile ("push     w3");// save original data in w3
    __asm__ volatile ("mov      #0xFF00,w2");
    __asm__ volatile ("and      w0,w2,w2");
    __asm__ volatile ("lsr      w1,#8,w3");//
    __asm__ volatile ("mov	_el_cam_pixel_pointer,w0");
    __asm__ volatile ("ior      w2,w3,[w0++]");//
    __asm__ volatile ("mov	w0,_el_cam_pixel_pointer");
    __asm__ volatile ("clr	_TMR4");
    __asm__ volatile ("bclr.b	_IFS1bits,#5");//IFS1bits.T4IF = 0;
    __asm__ volatile ("pop	w3");// restore w3
    __asm__ volatile ("pop	w2");// restore w2
    __asm__ volatile ("nop	");
    __asm__ volatile ("bset.b	_T1CONbits+1,#7");// T1CONbits.TON = 1
    __asm__ volatile ("pop.d    w0");// restore w0,w1
}

/*


    __asm__ volatile ("push.d   w2");// save original data in w2,w3
    __asm__ volatile ("mov	_PORTD,w2");
    __asm__ volatile ("push.d   w0");// save original data in w0,w1
    __asm__ volatile ("mov	_PORTD,w3");
    __asm__ volatile ("mov.b	_PORTCbits,WREG");
    __asm__ volatile ("btst	w0,#4");// check bit4 of PORTC (CAM_VSYNC)
    __asm__ volatile ("bra	nz,VSYNC_HIGH");
__asm__ volatile ("VSYNC_LOW:");
    __asm__ volatile ("bclr.b	_IFS1bits,#5");//IFS1bits.T4IF = 0;
    __asm__ volatile ("clr	_TMR4");
    __asm__ volatile ("bclr.b	_T4CONbits+1,#7");// T4CONbits.TON = 0
    __asm__ volatile ("pop.d    w0");// restore w0,w1
    __asm__ volatile ("pop.d    w2");// restore w2,w3
    __asm__ volatile ("retfie");
__asm__ volatile ("VSYNC_HIGH:");
    __asm__ volatile ("clr	_TMR4");
    __asm__ volatile ("mov	_el_cam_pixel_pointer,w0");
    __asm__ volatile ("inc2	_el_cam_pixel_pointer");// el_cam_pixel_pointer++
    __asm__ volatile ("bset.b	_T1CONbits+1,#7");// T1CONbits.TON = 1
    __asm__ volatile ("bclr.b	_IFS1bits,#5");//IFS1bits.T4IF = 0;
    __asm__ volatile ("mov      #0xFF00,w1");// prepare the mask of the high byte (first 8 bits)
    __asm__ volatile ("and      w1,w2,w2");// masking for the high byte (keep only first 8 bits)
    __asm__ volatile ("lsr      w3,#8,w3");// shift the low byte to the low 8 bits (keep only last 8 bits)
    __asm__ volatile ("ior      w2,w3,[w0]");// merge the high byte and the low byte of the pixel
    __asm__ volatile ("pop.d    w0");// restore w0,w1
    __asm__ volatile ("pop.d    w2");// restore w2,w3
    __asm__ volatile ("retfie");



*/

