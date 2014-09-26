
#include "el_context.h"

void el_init_interrupt_INT0(){
    
    REMOTE_DIR = INPUT_PIN;// set the RF6 pin as input
    
    INTCON2bits.INT0EP = 1;// set interrupt polarity to falling edge
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;// enable INT0
    IPC0bits.INT0IP = 4;// INT0 Interrupt Priority
    
}
    
void __attribute__((__interrupt__, auto_psv)) _INT0Interrupt(void){
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 0;// disable INT0
    el_irrc_phase = 1;
}
