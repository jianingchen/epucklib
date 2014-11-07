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

\defgroup EL_RANDOM Random Generator

\section Introduction

This is a Pseudo random generator module. Sensor values can be used 
to scramble the random seed to make each robot in a group obtain 
a virtually unique random seed. 

*/
 
/*!

\file
\ingroup EL_RANDOM

\author Jianing Chen

*/

#ifndef EL_RANDOM_H
#define	EL_RANDOM_H

#include "el_common.h"

/*! 
    \brief set seed of the random generator module
    
    \param seed     the random seed
    
    In a Pseudo random generator like this, the random seed defines 
    the sequence of values generated. Nth values of the random generators 
    using the same seed are same. 
*/
void el_random_set_seed(int seed);


/*! 
    \brief reset the random seed to default value
    
*/
void el_random_reset_seed(void);


/*! 
    \brief get seed of the current random sequence
    
    \return the random seed
*/
el_uint16 el_random_get_seed(void);


/*! 
    \brief returning a random unsigned short int in [0,65535]
    
    \return the unsigned short int
*/
el_uint16 el_random_uint16(void);


/*! 
    \brief returning a random unsigned long int in [0,2^32 - 1]
    
    \return the unsigned long int
*/
el_uint32  el_random_uint32(void);


/*! 
    \brief returning a random int in [min,max]

    \return the int
*/
el_int16 el_random_int(el_int16 min,el_int16 max);


/*! 
    \brief returning true at a probablity of (num/den)

    \param num      numerator of the probablity
    \param den      denominator of the probablity
    
    \return a Boolean result

    This function has a (num/den) chance of returning true. 
    It can be used in probablity based branch. For example:
    \code
    ...
    if(el_random_rate(1,5)){
        
        // do A (20 percent of chance)
        
    }else{
        
        // do B (80 percent of chance)
        
    }
    ...
    \endcode
*/
el_bool el_random_rate(el_uint16 num,el_uint16 den);

#endif
