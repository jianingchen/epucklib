
#include "el_context.h"
#include "el_camera.h"

#ifdef _BUILD_CONCEPT
// set PATH = %PATH%;C:\Program Files (x86)\Microchip\xc16\v1.21\bin
// xc16-gcc el_interrupt_T4.c -c -mcpu=30F6014A -O1 -S -D_BUILD_CONCEPT -o el_interrupt_T4_concept.s

void __attribute__((interrupt, no_auto_psv))_T4Interrupt(void){
    TMR4 = 0;
    IFS1bits.T4IF = 0;

    if(el_cam_y<15){

        el_cam_pixel_pointer = el_cam_w_frame->data[el_cam_y];

        T1CONbits.TON = 1;

        el_cam_line_pointer = el_cam_w_frame->data[el_cam_y];


    }else{

        T4CONbits.TON = 0;

        el_cam_x = el_cam_pixel_pointer - el_cam_line_pointer;

        el_cam_w_frame->width = el_cam_x;
        el_cam_w_frame->height = el_cam_y;

        el_cam_x = 0;
        el_cam_y = 0;

        if(!el_cam_lock_buffer){
            el_cam_swap_buffer();
        }
        
    }

    el_cam_y++;

}

#else

void el_init_interrupt_T4(){

    TMR4 = 0;
    PR4 = 1;
    IFS1bits.T4IF = 0;
    IEC1bits.T4IE = 1;//T4 Interrupt Enabled
    IPC5bits.T4IP = 6;//T4 Interrupt Priority
    T4CONbits.TCKPS = 1;//External Clock Prescsaler
    T4CONbits.TCS = 1;//External Clock
    T4CONbits.TON = 0;//Initially OFF

}

// line interrupt

void __attribute__((interrupt, no_auto_psv))_T4Interrupt(void){
    __asm__ volatile ("push.d   w0");// save original data in w0,w1
    __asm__ volatile ("push.d   w2");// save original data in w2,w3
    __asm__ volatile ("");
    __asm__ volatile ("mov	_el_cam_y,w3");
    __asm__ volatile ("sub	w3,#14,w0");
    __asm__ volatile ("bra	gtu,FINISH_FRAME");
__asm__ volatile ("START_LINE:");
    __asm__ volatile ("mov	_el_cam_w_frame,w0");
    __asm__ volatile ("add	w0,#4,w0");// w0: pointer to el_cam_w_frame->data
    __asm__ volatile ("mov	#80,w1");
    __asm__ volatile ("mul.ss	w3,w1,w2");// w1: el_cam_y*80
    __asm__ volatile ("add	w0,w2,w0");// w0 = pointer to el_cam_w_frame->data + el_cam_y*80
    __asm__ volatile ("mov	w0,_el_cam_pixel_pointer");
    __asm__ volatile ("bset.b	_T1CONbits+1,#7");// T1CONbits.TON = 1;
    __asm__ volatile ("mov	w0,_el_cam_line_pointer");
    __asm__ volatile ("inc	_el_cam_y");
    __asm__ volatile ("bra	END");
__asm__ volatile ("FINISH_FRAME:");
    __asm__ volatile ("bclr.b	_T4CONbits+1,#7");// T4CONbits.TON = 0;
    __asm__ volatile ("mov	_el_cam_line_pointer,w0");
    __asm__ volatile ("mov	_el_cam_pixel_pointer,w1");
    __asm__ volatile ("sub	w1,w0,w2");
    __asm__ volatile ("asr	w2,w2");
    __asm__ volatile ("mov	w2,_el_cam_x");// w2 and w3 are cam_x and cam_y
    __asm__ volatile ("mov	_el_cam_w_frame,w0");
    __asm__ volatile ("mov	w2,[w0]");
    __asm__ volatile ("mov	w3,[w0+2]");
    __asm__ volatile ("clr	_el_cam_x");
    __asm__ volatile ("clr	_el_cam_y");
    __asm__ volatile ("inc	_el_cam_frame_counter");
    __asm__ volatile ("mov	_el_cam_lock_buffer,w1");
    __asm__ volatile ("sub.b	w1,#0,w1");
    __asm__ volatile ("bra	nz,END");
__asm__ volatile ("SWAP_BUFFER:");
    __asm__ volatile ("mov	_el_cam_r_frame,w1");
    __asm__ volatile ("mov	w0,_el_cam_r_frame");
    __asm__ volatile ("mov	w1,_el_cam_w_frame");
    __asm__ volatile ("inc	_el_trg_event_flag_ex_cam");// signal a trigger event
__asm__ volatile ("END:");
    __asm__ volatile ("bclr.b	_IFS1bits,#5");// IFS1bits.T4IF = 0;
    __asm__ volatile ("pop.d    w2");// restore w2,w3
    __asm__ volatile ("pop.d    w0");// restore w0,w1
}

#endif
