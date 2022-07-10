#include "appLED.h"
#include "nrf_gpio.h"


void led_init(void){
	nrf_gpio_cfg_output(LED);
}	


void led_on(int pin){
	nrf_gpio_pin_set(pin);
}	

void led_off(int pin){
	nrf_gpio_pin_clear(pin);
}	

void led_toggle(int pin){
	nrf_gpio_pin_toggle(pin);
}	