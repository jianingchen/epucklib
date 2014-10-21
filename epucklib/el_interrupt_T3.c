
#include "el_context.h"
#include "el_clock.h"
#include "el_stepper_motor.h"
#include "el_ir_receiver.h"

void el_init_interrupt_T3(){
    
    IFS0bits.T3IF = 0;// clear interrupt flag
    IEC0bits.T3IE = 1;// enable interrupt
    IPC1bits.T3IP = 3;// T3 Interrupt Priority

    T3CON = 0;
    TMR3 = 0;
    PR3 = 128;// 14745600/8/128 = 14400
    T3CONbits.TCKPS = 1;// prescsaler = 8
    T3CONbits.TON = 1;// start Timer3

}

// 14400 Hz
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    
    IFS0bits.T3IF = 0;
    
    el_routine_masterclock_14400hz();

    if(el_stpm_enabled){
        el_routine_stepper_motor_14400hz();
    }
    
    el_routine_ir_receiver_14400hz();
    
    //el_routine_ir_proximity_noise();
    
}
