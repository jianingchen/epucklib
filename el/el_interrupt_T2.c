
#include "el_context.h"
#include "el_accelerometer.h"
#include "el_ir_proximity.h"

void el_init_interrupt_T2(){
    
    IFS0bits.T2IF = 0;// clear interrupt flag
    IEC0bits.T2IE = 1;// enable interrupt
    IPC1bits.T2IP = 3;// T2 Interrupt Priority
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 96;// 14745600/64/96 = 2400
    T2CONbits.TCKPS = 2;//prescsaler = 64
    T2CONbits.TON = 1;// turn on the timer
    
}

// 2400 Hz
void __attribute__((interrupt, auto_psv)) _T2Interrupt(void){
    unsigned int n;
    
    IFS0bits.T2IF = 0;


    // scheduling routines of sensors

    if(el_acc_enabled){
        el_routine_accelerometer_2400hz();
    }
    
    if(el_irps_enabled){
        el_routine_ir_proximity_2400hz();
    }
    


    // configure channels to be scanned

    n = 0;

    if(el_adc_callback_accelerometer){
        ADCSSLbits.CSSL5 = 1;   // axe x acc
        ADCSSLbits.CSSL6 = 1;   // axe y acc
        ADCSSLbits.CSSL7 = 1;   // axe z acc
        n += 3;
    }
    
    if(el_adc_callback_ir_proximity){
        ADCSSLbits.CSSL8 = 1;   // ir0
        ADCSSLbits.CSSL9 = 1;   // ir1
        ADCSSLbits.CSSL10 = 1;  // ir2
        ADCSSLbits.CSSL11 = 1;  // ir3
        ADCSSLbits.CSSL12 = 1;  // ir4
        ADCSSLbits.CSSL13 = 1;  // ir5
        ADCSSLbits.CSSL14 = 1;  // ir6
        ADCSSLbits.CSSL15 = 1;  // ir7
        n += 8;
    }


    
    // initiate adc scanning when there are requests

    if(n){

        IEC0bits.ADIE = 1;
        ADCON2bits.SMPI = n;// total number of channels to be scanned
        ADCON1bits.ADON = 1;
        
    }
    
    /*
    For what happens when the scanning is finished,
    see function "_ADCInterrupt" in "el_interrupt_ADC.c".
    */

}
