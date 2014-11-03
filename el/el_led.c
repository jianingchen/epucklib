
#include "el_context.h"
#include "el_led.h"

#define EL_DEAL_WITH_IT(v)\
    if(action==EL_TOGGLE){\
        v = v^1;\
    }else{\
        v = action;\
    }

void el_led_set(el_index which,el_led_action action){
    
    switch(which){

        case EL_LED_RING_0:
        EL_DEAL_WITH_IT(LED0);
        break;

        case EL_LED_RING_1:
        EL_DEAL_WITH_IT(LED1);
        break;

        case EL_LED_RING_2:
        EL_DEAL_WITH_IT(LED2);
        break;

        case EL_LED_RING_3:
        EL_DEAL_WITH_IT(LED3);
        break;

        case EL_LED_RING_4:
        EL_DEAL_WITH_IT(LED4);
        break;

        case EL_LED_RING_5:
        EL_DEAL_WITH_IT(LED5);
        break;

        case EL_LED_RING_6:
        EL_DEAL_WITH_IT(LED6);
        break;

        case EL_LED_RING_7:
        EL_DEAL_WITH_IT(LED7);
        break;

        case EL_LED_RING_ALL:
        if(action==EL_TOGGLE){
            LED0 = LED0^1;
            LED1 = LED1^1;
            LED2 = LED2^1;
            LED3 = LED3^1;
            LED4 = LED4^1;
            LED5 = LED5^1;
            LED6 = LED6^1;
            LED7 = LED7^1;
        }else{
            LED0 = action;
            LED1 = action;
            LED2 = action;
            LED3 = action;
            LED4 = action;
            LED5 = action;
            LED6 = action;
            LED7 = action;
        }
        break;

        case EL_LED_BODY:
        if(action==EL_TOGGLE){
            BODY_LED = BODY_LED^1;
        }else{
            BODY_LED = action;
        }
        break;

        case EL_LED_FRONT:
        if(action==EL_TOGGLE){
            FRONT_LED = FRONT_LED^1;
        }else{
            FRONT_LED = action;
        }
        break;

        default:
        break;
    }
}

#undef EL_DEAL_WITH_IT

int el_get_selector_value(){
    return (SELECTOR3<<3)|(SELECTOR2<<2)|(SELECTOR1<<1)|SELECTOR0;
}
