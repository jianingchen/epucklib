
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

#ifdef _BUILD_CONCEPT

// xc16-gcc el_interrupt_T1.c -c -mcpu=30F6014A -O1 -S -D_BUILD_CONCEPT -o el_interrupt_T1_concept.s

void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void){
    register unsigned int W;
    register unsigned int H,L;

    IFS0bits.T1IF = 0;

    H = CAM_DATA;
    L = CAM_DATA;

    if(CAM_HREF){

        W = (H&0xFF00)|(L>>8);

        *el_cam_pixel++ = W;

    }else{
        
        T1CONbits.TON = 0;
        T4CONbits.TON = 0;
        el_cam_swap_buffer();

    }
}

#else

void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void){
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
}

#endif

