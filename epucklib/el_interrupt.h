
#ifndef EL_INTERRUPT_H
#define EL_INTERRUPT_H

#ifdef EL_INCLUDE_CONTEXT

#define EL_T3_FREQ  14400

void el_init_interrupt_ADC();
void el_init_interrupt_INT0();
void el_init_interrupt_T1();
void el_init_interrupt_T2();
void el_init_interrupt_T3();
void el_init_interrupt_T4();
void el_init_interrupt_T5();

#endif

#endif