
#include "el_context.h"
#include "el_ir_proximity.h"
#include "el_accelerometer.h"

void el_init_interrupt_ADC(){

    // reset all bits
    ADCON1 = 0;
    ADCON2 = 0;
    ADCON3 = 0;
    
    // 0 for analog input mode, 
    // 1 for digital input mode (default)
    ADPCFGbits.PCFG0 = 1; // Debugger 
    ADPCFGbits.PCFG1 = 1; // Debugger 
    ADPCFGbits.PCFG2 = 0; // mic1
    ADPCFGbits.PCFG3 = 0; // mic2
    ADPCFGbits.PCFG4 = 0; // mic3
    ADPCFGbits.PCFG5 = 0; // axe x acc.
    ADPCFGbits.PCFG6 = 0; // axe y acc.
    ADPCFGbits.PCFG7 = 0; // axe z acc.
    ADPCFGbits.PCFG8 = 0; // ir0
    ADPCFGbits.PCFG9 = 0; // ir1
    ADPCFGbits.PCFG10 = 0;  // ir2
    ADPCFGbits.PCFG11 = 0;  // ir3
    ADPCFGbits.PCFG12 = 0;  // ir4
    ADPCFGbits.PCFG13 = 0;  // ir5
    ADPCFGbits.PCFG14 = 0;  // ir6
    ADPCFGbits.PCFG15 = 0;  // ir7
    
    // select the channels to be scanned
    ADCON2bits.CSCNA = 1;   // scan selected channel only
    ADCON2bits.SMPI = 0;   // number of channels to be sampled before done
    ADCSSLbits.CSSL0 = 0;   // Debugger
    ADCSSLbits.CSSL1 = 0;   // Debugger
    ADCSSLbits.CSSL2 = 0;   // micro 0
    ADCSSLbits.CSSL3 = 0;   // micro 1
    ADCSSLbits.CSSL4 = 0;   // micro 2
    ADCSSLbits.CSSL5 = 0;   // axe x acc
    ADCSSLbits.CSSL6 = 0;   // axe y acc
    ADCSSLbits.CSSL7 = 0;   // axe z acc
    ADCSSLbits.CSSL8 = 0;   // ir0
    ADCSSLbits.CSSL9 = 0;   // ir1
    ADCSSLbits.CSSL10 = 0;  // ir2
    ADCSSLbits.CSSL11 = 0;  // ir3
    ADCSSLbits.CSSL12 = 0;  // ir4
    ADCSSLbits.CSSL13 = 0;  // ir5
    ADCSSLbits.CSSL14 = 0;  // ir6
    ADCSSLbits.CSSL15 = 0;  // ir7
    
    ADCON1bits.ADON = 0;// adc scan initially off
    ADCON1bits.FORM = 0;// output data type: unsigned int
    ADCON1bits.ASAM = 1;// automatic sampling on
    ADCON1bits.SSRC = 0b111;// conversion triggered automaticly
    
    /*
     * [datasheet page-135]:
     *
     * The ADC conversion requires 14 TAD. The source of
     * the ADC conversion clock is software selected, using a
     * six-bit counter. There are 64 possible options for TAD.
     *
     * TAD = TCY*( 0.5*(ADCS<5:0> + 1) )
     *
     * The internal RC oscillator is selected by setting the
     * ADRC bit.
     * For correct ADC conversions, the ADC conversion
     * clock (TAD) must be selected to ensure a minimum TAD
     * time of 334 nsec (for VDD = 5V). Refer to the Electrical
     * Specifications section for minimum TAD under other
     * operating conditions.
     * Example 19-1 shows a sample calculation for the
     * ADCS<5:0> bits, assuming a device operating speed
     * of 30 MIPS.
     * 
    */
    
    ADCON3bits.ADCS = 19;// TAD = 680 nsec
    ADCON3bits.SAMC = 2;// number of ADC cycle between acquisition and conversion (need 2 for the prox)
    
    IFS0bits.ADIF = 0;// ADC Interrupt Flag
    IEC0bits.ADIE = 0;// ADC Interrupt Enable
    IPC2bits.ADIP = 2;// ADC Interrupt Priority
    
}

void __attribute__((__interrupt__, no_auto_psv)) _ADCInterrupt(void){
    unsigned int *p;
    
    ADCON1bits.ADON = 0;
    IFS0bits.ADIF = 0;
    IEC0bits.ADIE = 0;
    
    p = &ADCBUF0;// pointing to the first value in adc scanning result buffers
    
    if(el_adc_callback_accelerometer){
        el_adc_callback_accelerometer(p);
        p += 3;
        ADCSSLbits.CSSL5 = 0;   // axe x acc
        ADCSSLbits.CSSL6 = 0;   // axe y acc
        ADCSSLbits.CSSL7 = 0;   // axe z acc
    }
    
    if(el_adc_callback_ir_proximity){
        el_adc_callback_ir_proximity(p);
        p += 8;
        ADCSSLbits.CSSL8 = 0;   // ir0
        ADCSSLbits.CSSL9 = 0;   // ir1
        ADCSSLbits.CSSL10 = 0;  // ir2
        ADCSSLbits.CSSL11 = 0;  // ir3
        ADCSSLbits.CSSL12 = 0;  // ir4
        ADCSSLbits.CSSL13 = 0;  // ir5
        ADCSSLbits.CSSL14 = 0;  // ir6
        ADCSSLbits.CSSL15 = 0;  // ir7
    }
    
}
