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

#ifndef ELU_H
#define ELU_H

#include "el.h"
#include "elu_stdio.h"

#define MAX(a,b) (((a)>=(b))? (a):(b))
#define MIN(a,b) (((a)<(b))? (a):(b))

void elu_print_camera_image(const el_camera_image*p,el_uint32 n);

#endif
