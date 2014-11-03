
#include "el_context.h"
#include "el_interrupt.h"
#include "el_camera.h"
#include "el_trigger.h"

void el_init_interrupt_T5(){

    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 1;//T5 Interrupt Enabled
    IPC5bits.T5IP = EL_INTP_T5;//T5 Interrupt Priority
    
    TMR5 = 0;
    PR5 = 1;
    T5CONbits.TCKPS = 0;//External Clock Prescsaler
    T5CONbits.TCS = 1;//External Clock
    T5CONbits.TON = 0;//Initially OFF
    
}

// frame interrput
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(void){
    el_camera_image* temp;

    IFS1bits.T5IF = 0;
    TMR5 = 0;
    el_cam_frame_counter++;
    if(!el_cam_lock_buffer){
        // swap buffers
        temp = el_cam_w_frame;
        el_cam_w_frame = el_cam_r_frame;
        el_cam_r_frame = temp;
        // signal the camera trigger event
        el_trg_event_flag_ex_cam++;
    }
    el_cam_line_pointer = (uint16_t*)el_cam_w_frame->data;
    el_cam_pixel_pointer = el_cam_line_pointer;
    el_cam_y = 0;
    T4CONbits.TON = 1;

}
