
#ifndef EL_RANDOM_H
#define	EL_RANDOM_H

#include <stdint.h>

void el_random_set_seed(int seed);
void el_random_reset_seed(void);
uint16_t el_random_uint16(void);
uint32_t el_random_uint32(void);
int el_random_rate(int num,int den);// rate of (num/den) returing 1

#endif	/* EL_RANDOM_H */

