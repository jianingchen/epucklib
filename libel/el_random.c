
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

uint16_t el_random_uint16(void){
    el_rand_roll();
    return (m_z<<8) + (m_w&0xFF);
}

uint32_t el_random_uint32(void){
    el_rand_roll();
    return (m_z<<16) + (m_w&0xFFFF);
}

int el_random_rate(int num,int den){
    uint32_t r = el_random_uint16();
    return ((den*r)<(num*65536UL));
}
