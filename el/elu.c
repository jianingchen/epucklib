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
#include "el_process.h"
#include "elu.h"
#include "elu_stdio.h"

void elu_print_camera_image(const el_camera_image*frame,el_uint32 n){
    int X,Y,i;
    char *p;

    while(el_uart_is_sending(EL_UART_1)){
        el_process_cooperate();
    }
    
    el_uart_send_string(EL_UART_1,"<p>\n");

    p = elu_stdio_buffer;
    p += elu_snprintf(p,16,"NUM: %lu\n",n);
    p += elu_snprintf(p,16,"DIM: %d,%d\n",EL_CAMERA_FRAME_DIM_X,EL_CAMERA_FRAME_DIM_Y);
    p += elu_snprintf(p,16,"LEN: %d,%d\n",frame->Width,frame->Height);

    while(el_uart_is_sending(EL_UART_1)){
        el_process_cooperate();
    }
    el_uart_send_string(EL_UART_1,elu_stdio_buffer);

    for(Y=0;Y<EL_CAMERA_FRAME_DIM_Y;Y++){
        X = 0;
        while(X < EL_CAMERA_FRAME_DIM_X){

            p = elu_stdio_buffer;
            
            for(i=0;i<10;i++){
                p += elu_snprintf(p,16,"%4.4X",frame->Data[Y][X]);
                X++;
            }
            
            if(X==EL_CAMERA_FRAME_DIM_X){
                *p++ = '\n';
            }
            
            while(el_uart_is_sending(EL_UART_1)){
                el_process_cooperate();
            }
            el_uart_send_string(EL_UART_1,elu_stdio_buffer);
        }
    }

    while(el_uart_is_sending(EL_UART_1)){
        el_process_cooperate();
    }
    el_uart_send_string(EL_UART_1,"</p>\n");

}



