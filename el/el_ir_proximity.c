/**

embedded system library for e-puck

--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

**/

#include "el_context.h"
#include "el_ir_proximity.h"
#include "el_ir_receiver.h"
#include "el_random.h"

bool el_irps_enabled;
el_uint8 el_irps_working_mode;
el_uint8 el_irps_working_phase;
void (*el_adc_callback_ir_proximity)(const unsigned int*result_8v);
el_uint32 el_irps_counter;

el_bool el_irps_is_calibrated;
el_bool el_irps_subtract_ea;
el_uint16 el_irps_environment_ambient;

el_uint16 el_irps_samples_Ambient[8];//Noise + Ambient
el_uint16 el_irps_samples_Mixed[8];//Noise + Ambient + Emitter Reflection
el_uint16 el_irps_samples_Temp[8];
el_uint16 el_irps_samples_Last[8];
el_uint32 el_irps_samples_Spikes[8];
el_uint16 el_irps_samples_Noise[8];
el_uint16 el_irps_samples_NeutralReflection[8];// need calibration
el_uint16 el_irps_samples_NeutralNoise[8];

el_ir_proximity_param el_irps_parameters;

static void el_irps_set_ir_leds(int on){
    if(on){
        PULSE_IR0 = 1;//0,4 ON
        PULSE_IR1 = 1;//1,5 ON
        PULSE_IR2 = 1;//2,6 ON
        PULSE_IR3 = 1;//3,7 ON
    }else{
        PULSE_IR0 = 0;//0,4 OFF
        PULSE_IR1 = 0;//1,5 OFF
        PULSE_IR2 = 0;//2,6 OFF
        PULSE_IR3 = 0;//3,7 OFF
    }
}

static void el_irps_dump_adc_values(el_uint16*result8v,const unsigned int*adc_result_8v){
    int i;
    for(i=0;i<8;i++){
        result8v[i] = 4095 - adc_result_8v[i];
    }

}

void el_init_ir_proximity(){
    int i;

    PULSE_IR0 = 0;//Infrared Emitter 0, 4 OFF
    PULSE_IR1 = 0;//Infrared Emitter 1, 5 OFF
    PULSE_IR2 = 0;//Infrared Emitter 2, 6 OFF
    PULSE_IR3 = 0;//Infrared Emitter 3, 7 OFF
    
    el_irps_enabled = 0;
    el_irps_counter = 0;
    el_irps_working_mode = EL_IR_PROXIMITY_PULSE;
    el_irps_working_phase = 0;
    
    el_irps_is_calibrated = false;
    el_irps_subtract_ea = false;
    el_irps_environment_ambient = 0;
    
    el_adc_callback_ir_proximity = NULL;
    
    for(i=0;i<8;i++){
        el_irps_samples_Ambient[i] = 0;
        el_irps_samples_Mixed[i] = 0;
        el_irps_samples_Temp[i] = 0;
        el_irps_samples_Last[i] = 0;
        el_irps_samples_Noise[i] = 0;
        el_irps_samples_Spikes[i] = 0;
        el_irps_samples_NeutralReflection[i] = 5;
        el_irps_samples_NeutralNoise[i] = 15;
    }
    
    el_irps_parameters.WorkingMode = EL_IR_PROXIMITY_PULSE;
    el_irps_parameters.EnvironmentalAmbient = 0;
    el_irps_parameters.SubtractEA = false;
}

el_ir_proximity_param* el_config_ir_proximity_options(){
    return &el_irps_parameters;
}

void el_config_ir_proximity(const el_ir_proximity_param*p){

    el_irps_working_mode = p->WorkingMode;
    el_irps_environment_ambient = p->EnvironmentalAmbient;
    el_irps_subtract_ea = p->SubtractEA;
    
}

void el_enable_ir_proximity(void){
    if(!el_irps_enabled){
        el_irps_enabled = 1;
        el_irps_working_phase = el_random_int(4,79);
        if(el_irps_working_mode==EL_IR_PROXIMITY_EMIT){
            el_irps_set_ir_leds(1);
        }
    }
}

void el_disable_ir_proximity(void){
    int i;
    if(el_irps_enabled){
        el_irps_enabled = 0;
        el_irps_working_phase = 0;
        el_irps_set_ir_leds(0);
        el_adc_callback_ir_proximity = NULL;
        for(i=0;i<8;i++){
            el_irps_samples_Ambient[i] = 0;
            el_irps_samples_Mixed[i] = 0;
            el_irps_samples_Temp[i] = 0;
            el_irps_samples_Last[i] = 0;
            el_irps_samples_Noise[i] = 0;
            el_irps_samples_Spikes[i] = 0;
        }
    }
}


//------------------------------------------------------------------------------


static void el_irps_passive_adc_phase_0(const unsigned int*result_8v){
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Ambient,result_8v);
    el_irps_set_ir_leds(0);
}

void el_routine_ir_proximity_passive(void){

    switch(el_irps_working_phase){

    case 0:
        el_adc_callback_ir_proximity = el_irps_passive_adc_phase_0;
        break;
        
    case 1:
        ++el_irps_counter;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
    }

}


//------------------------------------------------------------------------------


static void el_irps_pulse_adc_phase_0(const unsigned int*result_8v){
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Ambient,result_8v);
}

static void el_irps_pulse_adc_phase_2(const unsigned int*result_8v){
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Mixed,result_8v);
    el_irps_set_ir_leds(0);
}

void el_routine_ir_proximity_pulse(void){
    
    switch(el_irps_working_phase){

    case 0:
        el_adc_callback_ir_proximity = el_irps_pulse_adc_phase_0;
        break;

    case 1:
        el_irps_set_ir_leds(1);
        break;
        
    case 2:
        el_adc_callback_ir_proximity = el_irps_pulse_adc_phase_2;
        break;
        
    case 3:
        ++el_irps_counter;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
        
    }
    
}


//------------------------------------------------------------------------------


static void el_irps_emit_adc_phase_0(const unsigned int*result_8v){
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Mixed,result_8v);
}

static void el_irps_emit_adc_phase_2(const unsigned int*result_8v){
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Ambient,result_8v);
    el_irps_set_ir_leds(1);
}

void el_routine_ir_proximity_emit(void){
    
    switch(el_irps_working_phase){
        
    case 0:
        el_adc_callback_ir_proximity = el_irps_emit_adc_phase_0;
        break;
        
    case 1:
        el_irps_set_ir_leds(0);
        break;
        
    case 2:
        el_adc_callback_ir_proximity = el_irps_emit_adc_phase_2;
        break;
        
    case 3:
        ++el_irps_counter;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
        
    }
    
}


//------------------------------------------------------------------------------


void el_irps_noise_adc_phase_0(const unsigned int*result_8v){
    
}

void el_routine_ir_proximity_noise(void){

}


//------------------------------------------------------------------------------


void el_routine_ir_proximity_2400hz(){

    /*
     * If the ir receiver is receiving a incoming message, ir proximtiy sensors
     * should do nothing for the sake of infrared interference.
     */
    if(el_irrc_phase){
        return;
    }

    /*
     * If the ir proximtiy sensors are in a sampling procedure, the ir receiver
     * should do nothing for the sake of infrared interference.
     */
    if(el_irps_working_phase==0){
        el_irrc_inhibit(1);
    }else
    if(el_irps_working_phase==3){
        el_irrc_inhibit(0);
    }
    
    // sampling procedure of the proximity sensors
    if(el_irps_working_phase<4){
        
        switch(el_irps_working_mode){

        case EL_IR_PROXIMITY_PASSIVE:
            el_routine_ir_proximity_pulse();
            break;

        case EL_IR_PROXIMITY_PULSE:
            el_routine_ir_proximity_pulse();
            break;

        case EL_IR_PROXIMITY_EMIT:
            el_routine_ir_proximity_emit();
            break;

        case EL_IR_PROXIMITY_COMMUNICATION:
            // (not implemented yet)
            break;

        }

    }
    
    // the rest of the phases just act as a time delay between two sampling procedures

    el_irps_working_phase++;
    if(el_irps_working_phase>=80){
        el_irps_working_phase = 0;
    }
}



static int el_irps_get_ambient(int i){
    int r;
    r = el_irps_samples_Ambient[i];
    if(el_irps_subtract_ea){
        r -= (int)el_irps_environment_ambient;
    }
    return (r < 0)? 0:r;
}

static int el_irps_get_reflection(int i){
    int r;
    r = (int)el_irps_samples_Mixed[i] - (int)el_irps_samples_Ambient[i];
    r -= (int)el_irps_samples_NeutralReflection[i];
    return (r < 0)? 0:r;
}

static int el_irps_get_noise(int i){
    int r;
    r = (int)el_irps_samples_Mixed[i] - (int)el_irps_samples_Ambient[i];
    r -= (int)el_irps_samples_NeutralReflection[i];
    if(r>2000){
        r = 0;
    }else{
        r = el_irps_samples_Noise[i] - el_irps_samples_NeutralNoise[i] - r;
        r = (r > 4095)? 4095:r;
        r = (r < 0)? 0:r;
    }
    return r;
}



el_uint32 el_ir_proximity_get_counter(){
    return el_irps_counter;
}

int el_ir_proximity_get(el_index index,el_ir_proximity_output u,el_int16*out){
    el_int16 length;
    int i;

    if(index==EL_IR_PROXIMITY_SENSOR_ALL){

        length = 0;
        for(i=0;i<8;i++){
            length += el_ir_proximity_get(i,u,out + length);
        }
        return length;

    }else{

        i = index%8;

        switch(u){

        case EL_IR_AMBIENT:
            *out = el_irps_get_ambient(i);
            return 1;

        case EL_IR_REFLECTION:
            *out = el_irps_get_reflection(i);
            return 1;

        case EL_IR_NOISE:
            *out = el_irps_get_noise(i);
            return 1;

        case EL_IR_ALL_3V:
            out[0] = el_irps_get_ambient(i);
            out[1] = el_irps_get_reflection(i);
            out[2] = el_irps_get_noise(i);
            return 3;

        }
        
    }

    return 0;
}
