/*

embedded system library for e-puck

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

#include "el_context.h"
#include "el_interrupt.h"
#include "el_stepper_motor.h"
#include "el_accelerometer.h"
#include "el_ir_proximity.h"
#include "el_timer.h"
#include "el_uart.h"

char el_error_string_buffer[16] = "<<ERROR-00>>";

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

    // speed control routine of stepper motors
    if(el_stpm_enabled&&el_stpm_accel_enabled){
        el_routine_stepper_motor_accel_2400hz();
    }
    
    // scheduling routines of sensors

    if(el_acc_enabled){
        el_routine_accelerometer_2400hz();
    }
    
    if(el_irps_enabled){
        el_routine_ir_proximity_2400hz();
    }
    
    // configure ADC channels to be scanned
    
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

    // time-out checking

    if(el_is_in_process_call){
        ++el_process_overwatch;
        if(el_process_overwatch>=120){
            el_error_signal = EL_SYS_ERROR_PROCESS_TIME_OUT;
            el_process_overwatch = 0;
        }
    }

    if(el_is_in_trigger_condition){
        ++el_trigger_overwatch;
        if(el_trigger_overwatch>=120){
            el_error_signal = EL_SYS_ERROR_TRIGGER_TIME_OUT;
            el_trigger_overwatch = 0;
        }
    }
    
    if(el_is_in_timer_callback){
        ++el_timer_overwatch;
        if(el_timer_overwatch>=120){
            el_error_signal = EL_SYS_ERROR_TIMER_TIME_OUT;
            el_timer_overwatch = 0;
        }
    }
    
    // error feedback
    if(el_error_signal){
        el_error_string_buffer[8] = '0' + el_error_signal/10;
        el_error_string_buffer[9] = '0' + el_error_signal%10;
        IPC2bits.U1TXIP = 7;// raise UART1 trasmitting interrupt priority
        el_uart_send_string(EL_UART_1,el_error_string_buffer);
        while(el_uart_is_sending(EL_UART_1)){
            el_nop();
        }
        IPC2bits.U1TXIP = EL_INTP_U1TX;
        el_nop_delay(2000);
        el_error_signal = 0;
    }

}
