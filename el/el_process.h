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

#define EL_PROCESS_MAX_NUM        7

typedef void (*el_process)(void*);
#define EL_PROCESS void

/*!
    \brief execute a function in a process
    
    \param func     the pointer to the function to be executed (entry function)
    \param arg      an arguments passed to the function to be executed
    
    \return the index of the process (-1 if failed)

    This function can execute a function (entry function) in a process
    with the given argument (as "func(arg);"). Maxmum number of processes is 7.
    
    The entry function must be a function that takes a void pointer or a 
    \c el_handle as the argument and returns void. For example:
    \code
    void my_process(void*arg){
        ...
    }
    \endcode
*/
el_index el_launch_process(el_process func,void*arg);


/*!
    \brief wait for a period of time
    
    \param t_ms time in millisecond

    This function induces a time delay. It can only be used in a process. 
*/
void el_process_wait(el_time t_ms);


/*!
    \brief wait for a period of time specified as a fraction
    
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
    For example, the following scheme can be used to waiting for a condition 
    to become true: 
    \code
    ...
    // wait for a condition
    do{
        el_process_cooperate();
    }while(some_condition_is_not_true());
    ...
    \endcode
    
    There could be a section in a process that simply cost large amount of 
    computation and thus make the process alone occupy the CPU for a long time. 
    "el_process_cooperate" should be called in appropriate positions in a 
    process of such types. For instance, in a triple-nested loop with a loop 
    body that is also time-costly, add a "el_process_cooperate();" after the 
    middle loop: 
    \code
    ...
    // some big computation program
    for(k=0;k<15;k++){
        for(j=0;j<40;j++){
            for(i=0;i<10;i++){
                some_complicated_algorithm(i,j,k);
            }
        }
        el_process_cooperate();
    }
    ...
    \endcode
*/
void el_process_cooperate();


/*!
    \brief finish the waiting period of a process immediately

    \param process_idx      the index of the process

    This function can make a process in ::el_process_wait finish the waiting
    period immediately. 
*/
void el_process_resume(el_index process_idx);


/*!
    \brief kill the process (the abnormal way to end a process)

    \param process_idx      the index of the process to be killed

    This function can be used to forcefully stop a process while it is
    waiting (e.g. when it entered ::el_process_wait or ::el_process_cooperate).
    Unlike the relation between 'create' and 'delete', a launched process are
    not supposed to be killed. A process ends normally when its entry function
    returns (reach the end of the function).

    Note: Killing a process causes all the local variables within to be lost.
    If a dynamically created object is referable only through such a local
    variable, a memory leak is resulted (unable to 'free'/'delete'
    through the pointer/handle anymore).
*/
void el_kill_process(el_index process_idx);


/*!
    \brief get index of the process where this function is called
    
    \return index of the process

    When this function is not called within a process, it returns -1.
*/
el_index el_get_local_process_index();


/*
--------------------------------------------------------------------------------
*/


#ifdef EL_INCLUDE_LIB_INTERNAL_CONTEXT

#define EL_PROCESS_STACK_OFFSET 192
#define EL_PROCESS_STACK_SIZE   192
#define EL_PROCESS_DIM          EL_PROCESS_MAX_NUM

extern el_mct el_process_mck;
extern el_bool el_is_in_process;
extern el_bool el_is_in_process_call;
extern el_index el_cmt_current_process_index;
extern el_uint16 el_process_overwatch;

void el_init_process();
void el_routine_process();

#endif



#endif
