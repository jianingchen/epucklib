/*
 This header is only included by the components within this library.
*/
#ifndef EL_INTERNAL_CONTEXT_H
#define EL_INTERNAL_CONTEXT_H

#include "e_epuck_ports.h"
#include "e_init_port.h"
#include "e_I2C_master_module.h"
#include "e_I2C_protocol.h"

#define EL_EPUCK_FCY (14745600L) // = 7.3728e6 * 8 / 4
#define EL_EPUCK_MEMORY_SIZE    8192

#define EL_HANDLE_TO_POINTER(h) ((void*)h)
#define EL_POINTER_TO_HANDLE(p) ((el_handle)p)

#define EL_INCLUDE_CONTEXT

#endif
