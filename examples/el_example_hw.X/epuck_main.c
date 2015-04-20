
#include <el.h>
#include <elu.h>

int main(void){

    el_initialization();
    
    el_uart_use_reset_code(true,128);

    el_led_set(EL_LED_BODY,EL_ON);
    
    elu_printf("Hello World!\n");

    el_main_loop();

    return 0;
}
