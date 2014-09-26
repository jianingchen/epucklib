
#include "el_context.h"

void el_init_interrupt_ADC(){
    int i;
    float TAD_TCY;
    
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
    
    //specifie the channels to be scanned
    ADCON2bits.CSCNA = 1;   // scan selected channel only
    ADCON2bits.SMPI = 8;    // interrupt after sample 7 are taken
    ADCSSLbits.CSSL0 = 0;   // Debugger
    ADCSSLbits.CSSL1 = 0;   // Debugger
    ADCSSLbits.CSSL2 = 0;   // micro 0
    ADCSSLbits.CSSL3 = 0;   // micro 1
    ADCSSLbits.CSSL4 = 0;   // micro 2
    ADCSSLbits.CSSL5 = 0;   // axe x acc
    ADCSSLbits.CSSL6 = 0;   // axe y acc
    ADCSSLbits.CSSL7 = 1;   // axe z acc
    ADCSSLbits.CSSL8 = 1;   // ir0
    ADCSSLbits.CSSL9 = 1;   // ir1
    ADCSSLbits.CSSL10 = 1;  // ir2
    ADCSSLbits.CSSL11 = 1;  // ir3
    ADCSSLbits.CSSL12 = 1;  // ir4
    ADCSSLbits.CSSL13 = 1;  // ir5
    ADCSSLbits.CSSL14 = 1;  // ir6
    ADCSSLbits.CSSL15 = 1;  // ir7

    //ADCHS = 7;

    ADCON1bits.ADON = 0;// adc initially off
    ADCON1bits.FORM = 0;// output data type: unsigned int
    ADCON1bits.ASAM = 1;// automatic sampling on
    ADCON1bits.SSRC = 7;// conversion triggered automaticly

    //ADCON3bits.ADCS = (int)(2.0*FCY/(16384.0*(14+1)*8)-1);
    TAD_TCY = FCY*(668e-9);// TCY = 1/FCY
    ADCON3bits.ADCS = (int)(2*TAD_TCY - 1);
    ADCON3bits.SAMC = 2;// number of ADC cycle between acquisition and conversion (need 2 for the prox)
    
    IFS0bits.ADIF = 0;// ADC Interrupt Flag
    IEC0bits.ADIE = 1;// ADC Interrupt Enable
    IPC2bits.ADIP = 2;// ADC Interrupt Priority

}

void __attribute__((__interrupt__, auto_psv)) _ADCInterrupt(void){
    int i;
    int h,d;
    unsigned long f;
    
    IFS0bits.ADIF = 0;
    ADCON1bits.ADON = 0;
    
    if(el_irps_working_mode==EL_IR_PROXIMITY_NOISE){
        ProximitySensorsTemp[0] = ADCBUF1;
        ProximitySensorsTemp[1] = ADCBUF2;
        ProximitySensorsTemp[2] = ADCBUF3;
        ProximitySensorsTemp[3] = ADCBUF4;
        ProximitySensorsTemp[4] = ADCBUF5;
        ProximitySensorsTemp[5] = ADCBUF6;
        ProximitySensorsTemp[6] = ADCBUF7;
        ProximitySensorsTemp[7] = ADCBUF8;
        for(i=0;i<8;i++){
            h = 4095 - ProximitySensorsTemp[i];
            d = h - ProximitySensorsLast[i];
            ProximitySensorsLast[i] = h;
            if(d>0){
                ProximitySensorsSpikes[i] += d;
            }
        }
    }
    
}
