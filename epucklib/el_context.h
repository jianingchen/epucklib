/*
 This header is only included by the components within this library.
*/
#ifndef EL_CONTEXT_H
#define EL_CONTEXT_H

#include "e_all_official_headers.h"

#define EL_EPUCK_FCY (14745600L) // = 7.3728e6 * 8 / 4
#define EL_EPUCK_MEMORY_SIZE    8192

#define EL_HANDLE_TO_POINTER(h) ((void*)h)
#define EL_POINTER_TO_HANDLE(p) ((el_handle)p)

#define EL_INCLUDE_CONTEXT

#endif
