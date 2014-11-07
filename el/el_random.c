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
#include "el_random.h"

int el_rand_seed = 0;
uint32_t m_w = 521288629UL;
uint32_t m_z = 362436069UL;// MWC Algorithm

void el_rand_roll(){
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
}

void el_random_set_seed(int seed){
    el_rand_seed = seed;
    m_w += seed;
    m_z -= seed;
}

void el_random_reset_seed(void){
    el_rand_seed = 0;
    m_w = 521288629;
    m_z = 362436069;
}

el_uint16 el_random_get_seed(void){
    return el_rand_seed;
}

el_uint16 el_random_uint16(void){
    el_rand_roll();
    return (m_z<<8) + (m_w&0xFF);
}

el_uint32 el_random_uint32(void){
    el_rand_roll();
    return (m_z<<16) + (m_w&0xFFFF);
}

el_int16 el_random_int(el_int16 min,el_int16 max){
    el_int32 s = max - min + 1;
    el_int32 r = el_random_uint16();
    return r*s/65536UL + min;
}

el_bool el_random_rate(el_uint16 num,el_uint16 den){
    uint32_t r = el_random_uint16();
    return ((den*r)<(num*65536UL));
}
