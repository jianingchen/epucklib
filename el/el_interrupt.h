
#ifndef EL_INTERRUPT_H
#define EL_INTERRUPT_H

#ifdef EL_INCLUDE_CONTEXT

// define interrupt priority
#define EL_INTP_ADC     1
#define EL_INTP_INT0    7
#define EL_INTP_T1      7
#define EL_INTP_T2      1
#define EL_INTP_T3      2
#define EL_INTP_T4      6
#define EL_INTP_T5      5
#define EL_INTP_U1RX    4
#define EL_INTP_U1TX    3
#define EL_INTP_U2RX    2
#define EL_INTP_U2TX    1


void el_init_interrupt_ADC();
void el_init_interrupt_INT0();
void el_init_interrupt_T1();
void el_init_interrupt_T2();
void el_init_interrupt_T3();
void el_init_interrupt_T4();
void el_init_interrupt_T5();
void el_init_interrupt_UART();

#endif

#endif
