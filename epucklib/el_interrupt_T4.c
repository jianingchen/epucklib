
#include "el_context.h"
#include "el_interrupt.h"

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

#ifdef _BUILD_CONCEPT

// xc16-gcc el_interrupt_T4.c -c -mcpu=30F6014A -S -D_BUILD_CONCEPT -o el_interrupt_T4_concept.s

void __attribute__((interrupt, no_auto_psv))_T4Interrupt(void){
    register unsigned int W;
    register unsigned int H,L;

    IFS1bits.T4IF = 0;
    TMR4 = 0;

    if(CAM_VSYNC){

        H = CAM_DATA;
        L = CAM_DATA;
        W = (H&0xFF00)|(L>>8);
        
        el_cam_x = el_cam_pixel - el_cam_line;
        
        el_cam_line = el_writing_frame->RawData[el_cam_y];
        el_cam_pixel = el_cam_line;

        *el_cam_pixel++ = W;

        T1CONbits.TON = 1;

        el_cam_y++;

    }else{
        
        el_cam_x = el_cam_pixel - el_cam_line;

        T4CONbits.TON = 0;

    }

}

#else

// xc16-gcc el_interrupt_T4.c -c -mcpu=30F6014A -S -o el_interrupt_T4.s

void __attribute__((interrupt, no_auto_psv))_T4Interrupt(void){

    IFS1bits.T4IF = 0;

    if(el_cam_y<15){
        
        el_cam_pixel = el_writing_frame->RawData[el_cam_y];
        
        T1CONbits.TON = 1;
        
        el_cam_line = el_writing_frame->RawData[el_cam_y];

        
    }else{

        T4CONbits.TON = 0;

        el_cam_x = el_cam_pixel - el_cam_line;
        
        el_writing_frame->dim_x = el_cam_x;
        el_writing_frame->dim_y = el_cam_y;

        if(!el_cam_lock_buffer){
            el_cam_swap_buffer();
        }
        
        el_cam_x = 0;
        el_cam_y = 0;
        
    }
    
    el_cam_y++;

}

#endif
