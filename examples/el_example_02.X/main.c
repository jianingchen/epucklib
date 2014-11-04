
#include <el.h>
#include <elu.h>

int main(int argc,char*argv[]){

    el_initialization();

    el_led_set(EL_LED_BODY,EL_ON);
    
    elu_printf("Hello World!\n");

    el_main_loop();

    return 0;
}
