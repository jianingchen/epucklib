/**

This header lets the components of this lib access everything. 

**/
#ifndef EL_CONTEXT_H
#define EL_CONTEXT_H

#include "e_all_official_headers.h"

#define EL_EPUCK_IPS (14745600L) // = 7.3728e6 * 8 / 4
#define EL_EPUCK_MEMORY_SIZE    8192

#define EL_INCLUDE_CONTEXT
#include "el.h"

#define EL_HANDLE_KEY  0xABCD
#define EL_HANDLE_TO_POINTER(h) ((void*)((h)^EL_HANDLE_KEY))
#define EL_POINTER_TO_HANDLE(p) (((el_handle)(p))^EL_HANDLE_KEY)

#endif
