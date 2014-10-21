
/*!

\defgroup EL_PROCESS Process

\section Introduction

A Process runs concurrently with other process. 

\section Usage

Call the function "el_create_timer" to create a timer in the system. 
Use the handle returned by this function to refer to the created timer 
in the related function. 

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

EL_API int el_launch_process(el_process function,void*appended_data);
EL_API void el_process_wait(el_time time_ms);
EL_API void el_process_wait_fraction(unsigned int num,unsigned int den);// (num/den) second
EL_API void el_process_cooperate();



#ifdef EL_INCLUDE_CONTEXT

extern el_mci el_process_mck;
extern bool el_is_in_process;

void el_init_process();
void el_routine_process();

#endif



#endif
