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

/*!

\file
\ingroup EL_PROCESS

\author Jianing Chen

*/

#ifndef EL_PROCESS_H
#define	EL_PROCESS_H

#include "el_common.h"
#include "el_clock.h"

typedef void (*el_process)(void*);
#define EL_PROCESS void

/*!
    \brief Execute a function as a process. 
    
    \param func     the pointer to the function to be executed
    \param arg      an arguments passed to the function to be executed
    
    \return the index of the process (-1 if failed)

    This function can execute a function (entry function) as a process 
    with the given argument (as "func(arg);"). Maxmum number of process is 7. 
    
    The entry function must be a function that takes a void pointer as the 
    parameter and returns void. For example:
    \code
    void my_process(void*that_arg){
        ...
    }
    \endcode
*/
int el_launch_process(el_process func,void*arg);


/*!
    \brief Wait for a period of time.
    
    \param t_ms time in millisecond

    This function induces a time delay. It can only be used in a process. 
*/
void el_process_wait(el_time t_ms);


/*!
    \brief Wait for a period of time specified as a fraction.
    
    \param num numerator of the time in seconds.
    \param den denominator of the time in seconds.

    This function induces a time delay of (num/den) seconds in a process.
*/
void el_process_wait_fraction(unsigned int num,unsigned int den);


/*!
    \brief Introduce an atomic wait so other processes can be executed. 

    This function introduce an atomic delay, which give other processes  
    a chance to execute. Once this process get a chance to execute again, 
    it will continue execute. Therefore, unlike "el_process_wait", 
    "el_process_cooperate" does not cause a notable time delay. 
    It can be used in polling mechanism inside a process. 
    For example, one can write: 
    \code
    ...
    // wait for some condition
    while(some_condition_is_not_true()){
        el_process_cooperate();
    }
    ...
    \endcode
    
    "el_process_cooperate" is also used within a single process that 
    potentially cost large amount of computation and thus occupy the CPU for 
    a long time. Calling "el_process_cooperate" in appropriate positions in 
    a process of such types. For example, in a triple-nested loop (potentially 
    cost a long time to execute), add a "el_process_cooperate();" after the 
    middle loop: 
    \code
    ...
    // some big computation program
    for(k=0;k<15;k++){
        for(j=0;j<40;j++){
            for(i=0;i<10;i++){
                do_some_thing(i,j,k);
            }
        }
        el_process_cooperate();
    }
    ...
    \endcode
*/
void el_process_cooperate();


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_CONTEXT

#define EL_PROCESS_STACK_OFFSET 160
#define EL_PROCESS_STACK_SIZE   192
#define EL_PROCESS_DIM          7

extern el_mci el_process_mck;
extern bool el_is_in_process;

void el_init_process();
void el_routine_process();

#endif



#endif
