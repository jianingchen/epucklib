
#include "el_context.h"
#include "el_ir_proximity.h"

uint8_t el_irps_working_mode;
uint8_t el_irps_working_phase;
bool el_irps_enabled;

bool el_irps_is_in_sampling_routine;
volatile bool el_irps_is_waiting_result;
volatile el_ct el_irps_micro_timer;

uint16_t el_irps_adc_counter;
uint16_t el_irps_counter;

uint16_t el_irps_environment_ambient;
bool el_irps_is_calibrated;

uint16_t ProximitySensorsNeutral[8];
uint16_t ProximitySensorsNeutralNoise[8];
uint16_t ProximitySensorsAmbient[8];//Noise + Ambient
uint16_t ProximitySensorsMixed[8];//Noise + Ambient + Emitter Reflection

uint16_t ProximitySensorsTemp[8];
uint16_t ProximitySensorsLast[8];
uint16_t ProximitySensorsNoise[8];
uint32_t ProximitySensorsSpikes[8];



void el_init_ir_proximity(){
    int i;
    
    el_irps_enabled = 0;
    el_irps_working_mode = EL_IR_PROXIMITY_PULSE;
    el_irps_working_phase = 0;
    el_irps_adc_counter = 0;
    el_irps_counter = 0;
    el_irps_micro_timer = 0;
    
    el_irps_is_in_sampling_routine = 0;
    el_irps_is_waiting_result = 0;

    el_irps_is_calibrated = 0;
    el_irps_environment_ambient = 30;
    for(i=0;i<8;i++){
        ProximitySensorsNeutral[i] = 15;
        ProximitySensorsNeutralNoise[i] = 15;
        ProximitySensorsAmbient[i] = 0;
        ProximitySensorsMixed[i] = 0;
        ProximitySensorsTemp[i] = 0;
        ProximitySensorsLast[i] = 0;
        ProximitySensorsNoise[i] = 0;
        ProximitySensorsSpikes[i] = 0;
    }
    
    PULSE_IR0 = 0;//Infrared Emitter 0, 4 OFF
    PULSE_IR1 = 0;//Infrared Emitter 1, 5 OFF
    PULSE_IR2 = 0;//Infrared Emitter 2, 6 OFF
    PULSE_IR3 = 0;//Infrared Emitter 3, 7 OFF
    
}

void el_irps_set_ir_leds(int on){
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

void el_enable_ir_proximity(void){
    if(!el_irps_enabled){
        el_irps_enabled = 1;
        if(el_irps_working_mode==EL_IR_PROXIMITY_KEEP){
            el_irps_set_ir_leds(1);
        }
    }
}

void el_disable_ir_proximity(void){
    int i;
    if(el_irps_enabled){
        el_irps_enabled = 0;
        el_irps_set_ir_leds(0);
        for(i=0;i<8;i++){
            ProximitySensorsAmbient[i] = 0;
            ProximitySensorsMixed[i] = 0;
            ProximitySensorsTemp[i] = 0;
            ProximitySensorsLast[i] = 0;
            ProximitySensorsNoise[i] = 0;
            ProximitySensorsSpikes[i] = 0;
        }
    }
}

static void el_irps_read_adc_values(uint16_t*result8v){
    int i;
    uint16_t *p;

    p = result8v;
    
    *p++ = ADCBUF1;
    *p++ = ADCBUF2;
    *p++ = ADCBUF3;
    *p++ = ADCBUF4;
    *p++ = ADCBUF5;
    *p++ = ADCBUF6;
    *p++ = ADCBUF7;
    *p++ = ADCBUF8;
    
    for(i=0;i<8;i++){
        result8v[i] = 4095 - result8v[i];
    }
    
}

void el_start_adc_scan(){
    ADCON1bits.ADON = 1;
}

void el_routine_ir_proximity_pulse(){
    
    switch(el_irps_working_phase){
        
    case 0:
        el_start_adc_scan();
        break;
        
    case 1:
        el_irps_read_adc_values(ProximitySensorsAmbient);
        el_irps_set_ir_leds(1);
        el_start_adc_scan();
        break;
        
    case 2:
        el_irps_read_adc_values(ProximitySensorsMixed);
        el_irps_set_ir_leds(0);
        el_irps_counter++;
        break;
        
    }
    
}

void el_routine_ir_proximity_keep(){
    
    switch(el_irps_working_phase){
        
    case 0:
        el_start_adc_scan();
        break;
        
    case 1:
        el_irps_read_adc_values(ProximitySensorsMixed);
        el_irps_set_ir_leds(0);
        el_start_adc_scan();
        break;
        
    case 2:
        el_irps_read_adc_values(ProximitySensorsAmbient);
        el_irps_set_ir_leds(1);
        el_irps_counter++;
        break;
        
    }
    
}

void el_routine_ir_proximity_noise(){
    uint32_t d;
    int i;
    
    switch(el_irps_working_phase){
        
    case 0:
        el_irps_set_ir_leds(1);
        el_start_adc_scan();
        for(i=0;i<8;i++){
            d = ProximitySensorsSpikes[i]/16;
            ProximitySensorsSpikes[i] = 0;
            ProximitySensorsNoise[i] = 3*ProximitySensorsNoise[i]/4 + d/4;
        }
        break;
        
    case 1:
        el_irps_read_adc_values(ProximitySensorsTemp);
        el_irps_set_ir_leds(0);
        el_start_adc_scan();
        for(i=0;i<8;i++){
            d = ProximitySensorsTemp[i];
            ProximitySensorsMixed[i] = 3*ProximitySensorsMixed[i]/4 + d/4;
        }
        break;
        
    case 2:
        el_irps_read_adc_values(ProximitySensorsTemp);
        for(i=0;i<8;i++){
            d = ProximitySensorsTemp[i];
            ProximitySensorsAmbient[i] = 3*ProximitySensorsAmbient[i]/4 + d/4;
        }
        el_irps_counter++;
        break;
        
    }
    
}

void el_routine_ir_proximity(){
    
    if(!el_irps_enabled){
        return;
    }
    
    if(el_irps_working_phase<3){
        
        switch(el_irps_working_mode){
            
        case EL_IR_PROXIMITY_PULSE:
            el_routine_ir_proximity_pulse();
            break;
            
        case EL_IR_PROXIMITY_KEEP:
            el_routine_ir_proximity_keep();
            break;
            
        case EL_IR_PROXIMITY_NOISE:
            el_routine_ir_proximity_noise();
            break;
            
        }
        
    }
    
    el_irps_working_phase++;
    if(el_irps_working_phase>=24){
        el_irps_working_phase = 0;
    }
    
}

/*
void el_routine_ir_proximity_sample(){
    int i,j;
    uint32_t d;
    
    case EL_IR_PROXIMITY_NOISE:
        //turn on infrared LED
        PULSE_IR0 = 1;//0,4 ON
        PULSE_IR1 = 1;//1,5 ON
        PULSE_IR2 = 1;//2,6 ON
        PULSE_IR3 = 1;//3,7 ON
        //delay ~300 us, process other things by the way
        el_irps_micro_timer = 3;
        for(i=0;i<8;i++){
            d = ProximitySensorsSpikes[i]/16;
            ProximitySensorsSpikes[i] = 0;
            ProximitySensorsNoise[i] = 3*ProximitySensorsNoise[i]/4 + d/4;
        }
        while(el_irps_micro_timer) NOP();
        //sample ambient + reflected strength
        el_sample_proximity_sensors(ProximitySensorsTemp);

        //turn off infrared LED
        PULSE_IR0 = 0;//0,4 OFF
        PULSE_IR1 = 0;//1,5 OFF
        PULSE_IR2 = 0;//2,6 OFF
        PULSE_IR3 = 0;//3,7 OFF
        //delay ~300 us, process other things by the way
        el_irps_micro_timer = 3;
        for(i=0;i<8;i++){
            d = ProximitySensorsTemp[i];
            ProximitySensorsMixed[i] = 3*ProximitySensorsMixed[i]/4 + d/4;
        }
        while(el_irps_micro_timer) NOP();
        //sample ambient strength
        el_sample_proximity_sensors(ProximitySensorsTemp);
        for(i=0;i<8;i++){
            d = ProximitySensorsTemp[i];
            ProximitySensorsAmbient[i] = 3*ProximitySensorsAmbient[i]/4 + d/4;
        }
    
        break;
    }
    
    el_irps_is_in_sampling_routine = 0;
}
*/

/*

void el_routine_ir_proximity_noise(void){
    static el_ct c = 0;
    static el_ct k = 0;

    if(!el_irps_enabled){
        return;
    }
    if(el_irps_working_mode!=EL_IR_PROXIMITY_NOISE){
        return;
    }
    if(el_irps_is_in_sampling_routine){
        return;
    }
    
    //number_of_cycles++;
    
    if(k){
        k++;
        if(k>2){
            // keep 2 cycles
            k = 0;
            PULSE_IR0 = 0;
            PULSE_IR1 = 0;
            PULSE_IR2 = 0;
            PULSE_IR3 = 0;
        }
    }
    
    c++;
    if(c==6){
        c = 0;
        // every 6 cycles
        if(el_random_rate(1,2)){
            // rate of 0.5
            PULSE_IR0 = 1;
            PULSE_IR1 = 1;
            PULSE_IR2 = 1;
            PULSE_IR3 = 1;
            k = 1;
        }
    }
    
    // start a sample event
    el_irps_is_waiting_result = true;
    ADCON1bits.ADON = 1;
    
}

*/
