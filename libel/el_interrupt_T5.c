
#include "el_context.h"
#include "el_camera.h"

void el_init_interrupt_T5(){

    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 1;//T5 Interrupt Enabled
    IPC5bits.T5IP = 5;//T5 Interrupt Priority
    
    TMR5 = 0;
    PR5 = 1;
    T5CONbits.TCKPS = 0;//External Clock Prescsaler
    T5CONbits.TCS = 1;//External Clock
    T5CONbits.TON = 0;//Initially OFF
    
}

// frame interrput
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void){
    T4CONbits.TON = 1;
    IFS1bits.T5IF = 0;
    TMR5 = 0;
}
