
#include "el_context.h"
#include "el_interrupt.h"

void el_init_interrupt_T1(){
    
    TMR1 = 0;
    PR1 = 1;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;//T1 Interrupt Enabled
    IPC0bits.T1IP = 7;//T1 Interrupt Priority (highest in the system)
    T1CONbits.TCKPS = 1;//External Clock Prescsaler
    T1CONbits.TSYNC = 0;//Syncronised External Clock
    T1CONbits.TCS = 1;//External Clock
    T1CONbits.TON = 0;//Initially OFF

}

// pixel interrupt

#ifdef _BUILD_CONCEPT

// xc16-gcc el_interrupt_T1.c -c -mcpu=30F6014A -S -D_BUILD_CONCEPT -o el_interrupt_T1_concept.s

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

// xc16-gcc el_interrupt_T1.c -c -mcpu=30F6014A -S -o el_interrupt_T1.s

void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void){
    __asm__ volatile ("push.s   ");// save original data in w0,w1,w2,w3
    __asm__ volatile ("bclr.b   _IFS0bits,#3");// IFS0bits.T1IF = 0;
    __asm__ volatile ("mov      _PORTD,w0");// CAM_DATA
    __asm__ volatile ("mov	_PORTCbits,w3");// CAM_HREF
    __asm__ volatile ("mov      _PORTD,w1");// CAM_DATA
    __asm__ volatile ("and	w3,#8,w3");// mask _PORTC to get CAM_HREF
    __asm__ volatile ("sub      w3,#0,w3");// a dummy op to set flag reg
    __asm__ volatile ("bra      z,LINE_FINISH");// branch check if last op is zero
    __asm__ volatile ("mov      #0xFF00,w2");
    __asm__ volatile ("and      w0,w2,w0");
    __asm__ volatile ("lsr      w1,#8,w1");
    __asm__ volatile ("ior      w0,w1,w3");
    __asm__ volatile ("mov      _el_cam_pixel,w2");
    __asm__ volatile ("mov      w3,[w2]");
    __asm__ volatile ("inc2     w2,w2");
    __asm__ volatile ("mov      w2,_el_cam_pixel");
    __asm__ volatile ("bra      END");
__asm__ volatile ("LINE_FINISH:");
    __asm__ volatile ("bclr.b	_T1CONbits+1,#7");// T1CONbits.TON = 0;
    //__asm__ volatile ("mov      _el_cam_y,w0");
    //__asm__ volatile ("inc      w0,w0");
    //__asm__ volatile ("mov      w0,_el_cam_y");
__asm__ volatile ("END:");
    __asm__ volatile ("pop.s    ");// restore w0,w1,w2,w3
}

#endif

