
#include "el_context.h"
#include "el_ir_proximity.h"

bool el_irps_enabled;
uint8_t el_irps_working_mode;
uint8_t el_irps_working_phase;
void (*el_adc_callback_ir_proximity)(const unsigned int*result_8v);
uint16_t el_irps_counter;
bool el_irps_is_calibrated;
uint16_t el_irps_environment_ambient;

uint16_t el_irps_samples_Neutral[8];
uint16_t el_irps_samples_NeutralNoise[8];
uint16_t el_irps_samples_Ambient[8];//Noise + Ambient
uint16_t el_irps_samples_Mixed[8];//Noise + Ambient + Emitter Reflection
uint16_t el_irps_samples_Temp[8];
uint16_t el_irps_samples_Last[8];
uint16_t el_irps_samples_Noise[8];
uint32_t el_irps_samples_Spikes[8];

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

static void el_irps_dump_adc_values(uint16_t*result8v,const unsigned int*adc_result_8v){
    int i;
    for(i=0;i<8;i++){
        result8v[i] = 4095 - adc_result_8v[i];
    }

}

void el_init_ir_proximity(){
    int i;
    
    el_irps_enabled = 0;
    el_irps_counter = 0;
    el_irps_working_mode = EL_IR_PROXIMITY_MODE_PULSE;
    el_irps_working_phase = 0;
    
    el_irps_is_calibrated = 0;
    el_irps_environment_ambient = 30;
    
    el_adc_callback_ir_proximity = NULL;

    for(i=0;i<8;i++){
        el_irps_samples_Neutral[i] = 5;
        el_irps_samples_NeutralNoise[i] = 15;
        el_irps_samples_Ambient[i] = 0;
        el_irps_samples_Mixed[i] = 0;
        el_irps_samples_Temp[i] = 0;
        el_irps_samples_Last[i] = 0;
        el_irps_samples_Noise[i] = 0;
        el_irps_samples_Spikes[i] = 0;
    }
    
    PULSE_IR0 = 0;//Infrared Emitter 0, 4 OFF
    PULSE_IR1 = 0;//Infrared Emitter 1, 5 OFF
    PULSE_IR2 = 0;//Infrared Emitter 2, 6 OFF
    PULSE_IR3 = 0;//Infrared Emitter 3, 7 OFF
    
}

void el_config_ir_proximity(el_ir_proximity_mode mode){
    el_irps_working_mode = mode;
}

void el_enable_ir_proximity(void){
    if(!el_irps_enabled){
        el_irps_enabled = 1;
        el_irps_working_phase = 0;
        if(el_irps_working_mode==EL_IR_PROXIMITY_MODE_EMIT){
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

//==============================================================================

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
        el_irps_counter++;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
    }

}

//==============================================================================

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
        el_irps_counter++;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
        
    }
    
}

//==============================================================================

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
        el_irps_counter++;
        /// signal a trigger event
        el_trg_event_flag_ex_irps = 1;
        break;
        
    }
    
}

//==============================================================================

void el_irps_noise_adc_phase_0(const unsigned int*result_8v){
    /*
    int d,h;
    int i;
    el_adc_callback_ir_proximity = NULL;
    el_irps_dump_adc_values(el_irps_samples_Temp,result_8v);
    for(i=0;i<8;i++){
        // accumlate spike (rising edge) magnitude
        d = h - el_irps_samples_Last[i];
        el_irps_samples_Last[i] = h;
        if(d>0){
            el_irps_samples_Spikes[i] += d;
        }
    }
    */
}

void el_routine_ir_proximity_noise(void){
    /*
    uint32_t d;
    int i;
    
    switch(el_irps_working_phase){
        
    case 0:
        el_irps_set_ir_leds(1);
        el_start_adc_scan();
        for(i=0;i<8;i++){
            d = el_irps_samples_Spikes[i]/16;
            el_irps_samples_Spikes[i] = 0;
            el_irps_samples_Noise[i] = 3*el_irps_samples_Noise[i]/4 + d/4;
        }
        break;
        
    case 1:
        el_irps_dump_adc_values(el_irps_samples_Temp,result_8v);
        el_irps_set_ir_leds(0);
        el_start_adc_scan();
        for(i=0;i<8;i++){
            d = el_irps_samples_Temp[i];
            el_irps_samples_Mixed[i] = 3*el_irps_samples_Mixed[i]/4 + d/4;
        }
        break;
        
    case 2:
        el_irps_dump_adc_values(el_irps_samples_Temp,result_8v);
        for(i=0;i<8;i++){
            d = el_irps_samples_Temp[i];
            el_irps_samples_Ambient[i] = 3*el_irps_samples_Ambient[i]/4 + d/4;
        }
        el_irps_counter++;
        el_trg_event_handler_irps();
        break;
        
    default:
        break;
    }
    */
}

//==============================================================================

void el_routine_ir_proximity_2400hz(){
    
    if(el_irps_working_phase<4){
        
        switch(el_irps_working_mode){

        case EL_IR_PROXIMITY_MODE_PASSIVE:
            el_routine_ir_proximity_pulse();
            break;

        case EL_IR_PROXIMITY_MODE_PULSE:
            el_routine_ir_proximity_pulse();
            break;
            
        case EL_IR_PROXIMITY_MODE_EMIT:
            el_routine_ir_proximity_emit();
            break;
            
        case EL_IR_PROXIMITY_MODE_NOISE:
            el_routine_ir_proximity_noise();
            break;
            
        }
        
    }
    
    el_irps_working_phase++;
    if(el_irps_working_phase>=80){
        el_irps_working_phase = 0;
    }
}



int el_irps_get_ambient(int i){
    int r;
    r = el_irps_samples_Ambient[i] - el_irps_environment_ambient;
    return (r < 0)? 0:r;
}

int el_irps_get_reflection(int i){
    int r;
    r = el_irps_samples_Mixed[i] - el_irps_samples_Ambient[i] - el_irps_samples_Neutral[i];
    return (r < 0)? 0:r;
}

int el_irps_get_noise(int i){
    int r;
    r = el_irps_samples_Mixed[i] - el_irps_samples_Ambient[i] - el_irps_samples_Neutral[i];
    if(r>2000){
        r = 0;
    }else{
        r = el_irps_samples_Noise[i] - el_irps_samples_NeutralNoise[i] - r;
        r = (r > 4095)? 4095:r;
        r = (r < 0)? 0:r;
    }
    return r;
}



int el_ir_proximity_get_counter(){
    return el_irps_counter;
}

int el_ir_proximity_get(el_ir_proximity_output type,el_index which){
    int i = which%8;
    
    switch(type){
        
    case EL_IR_AMBIENT:
        return el_irps_get_ambient(i);
        
    case EL_IR_REFLECTION:
        return el_irps_get_reflection(i);
        
    case EL_IR_NOISE:
        return el_irps_get_noise(i);
        
    }

    return 0;
}

void el_ir_proximity_get_all(el_ir_proximity_output type,int*result_8v){
    int i;
    
    if(result_8v==NULL){
        return;
    }
    
    switch(type){
        
    case EL_IR_AMBIENT:
        for(i=0;i<8;i++){
            result_8v[i] = el_irps_get_ambient(i);
        }
        break;
        
    case EL_IR_REFLECTION:
        for(i=0;i<8;i++){
            result_8v[i] = el_irps_get_reflection(i);
        }
        break;
        
    case EL_IR_NOISE:
        for(i=0;i<8;i++){
            result_8v[i] = el_irps_get_noise(i);
        }
        break;
        
    }
    
}
