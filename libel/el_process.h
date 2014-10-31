
/*!

\defgroup EL_PROCESS Process

\section Introduction

A Process runs concurrently with other process. 

\section Usage

This library has multi-tasking capability as it can process 8 'Process' 
concurrently. Call the function "el_launch_process" to launch a function 
into a Process. In a Process, call "el_process_wait" to wait for a certain 
amount of time. Within this wait period, other process are being executed. 
This approach of multitasking is often called "Cooperative Multitasking". 
It is useful when several timing based programs need to run concurrently. 
See the example project for how & the suitable situation to use Process. 

*/
 
/*!

\file
\ingroup EL_PROCESS

\author Jianing Chen

*/

#ifndef EL_PROCESS_H
#define	EL_PROCESS_H

#include "el_common.h"
#include "el_clock.h"

// the entry function of a process is a function that takes a pointer and returns nothing
typedef void (*el_process)(void*);
#define EL_PROCESS void

/*!
    \brief Execute a function in a Process. 
    
    \param func the pointer to the function to be executed
    \param arg  an arguments passed to the function to be executed
    
    \return the index of the process running the function (-1 is fail)

    This function execute a function in a Process with the argument given 
    (like "func(arg);"). Maxmum number of process is 8. 
*/
int el_launch_process(el_process func,void*arg);


/*!
    \brief Wait for a period of time.
    
    \param t_ms time in millisecond

    This function wait for a period of time. It can only be used in a process, 
    which means within the launched function and the functions it calls. 
*/
void el_process_wait(el_time t_ms);


/*!
    \brief Wait for a period of time specified as a fraction.
    
    \param num numerator of the time period to be wait in seconds.
    \param den denominator of the time period to be wait in seconds.

    This function wait for (num/den) seconds in a process.
*/
void el_process_wait_fraction(unsigned int num,unsigned int den);


/*!
    \brief Introduce an atomic wait so other process can be executed. 

    This function introduce an atomic wait, which will let other process gain 
    a chance to be executed. Once this process get a chance to execute again, 
    it will continue execute. Therefore, unlike "el_process_wait", 
    "el_process_cooperate" does not caused a notable time delay. 
    It can be used in polling inside a process function. For example, 
    one can write: 
    \code
    ...
    // wait for some condition
    while(some_condition_is_not_true()){
        el_process_cooperate();
    }
    ...
    \endcode
    
    "el_process_cooperate" is also used to deal with a single process that 
    potentially cost large amount of computation and thus occupy the CPU for 
    a long time. Calling "el_process_cooperate" in appropriate positions in 
    a process of such types. For example, in a triple-nested loop (potentially 
    cost a long time to execute), add a "el_process_cooperate();" after the 
    middle loop. 
*/
void el_process_cooperate();



#ifdef EL_INCLUDE_CONTEXT

#define EL_PROCESS_STACK_OFFSET   128
#define EL_PROCESS_STACK_SIZE   176
#define EL_PROCESS_DIM          6

extern el_mci el_process_mck;
extern bool el_is_in_process;

void el_init_process();
void el_routine_process();

#endif



#endif
