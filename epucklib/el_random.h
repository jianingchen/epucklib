
#ifndef EL_RANDOM_H
#define	EL_RANDOM_H

#include <stdint.h>

EL_API void el_random_set_seed(int seed);
EL_API void el_random_reset_seed(void);
EL_API uint16_t el_random_uint16(void);
EL_API uint32_t el_random_uint32(void);
EL_API bool el_random_rate(int num,int den);// rate of (num/den) returing true

#endif	/* EL_RANDOM_H */

