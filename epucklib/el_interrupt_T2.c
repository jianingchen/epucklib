
#include "el_context.h"
#include "el_interrupt.h"

void el_init_interrupt_T2(){
    
    IFS0bits.T2IF = 0;// clear interrupt flag
    IEC0bits.T2IE = 1;// enable interrupt
    IPC1bits.T2IP = 1;// T2 Interrupt Priority
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 192;// 14745600/64/192 = 1200
    T2CONbits.TCKPS = 2;//prescsaler = 64
    T2CONbits.TON = 1;// turn on the timer
    
}

// 1200 Hz
void __attribute__((interrupt, auto_psv)) _T2Interrupt(void){
    
    IFS0bits.T2IF = 0;
    
    el_routine_ir_proximity();
    
}
