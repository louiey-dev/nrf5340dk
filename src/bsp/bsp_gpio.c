#include "bsp.h"
#include <zephyr/drivers/gpio.h>

struct gpio_dt_spec leds[NUM_LED] = { LED0, LED1, LED2, LED3 };

int bsp_gpio_init(void){
	int ret = 0;

    struct gpio_dt_spec led;

    for(int i = 0; i < 4; i++){
        led = leds[i];

        if (!gpio_is_ready_dt(&led)) {
            ERR("%s gpio_is_ready_dt is not ready...%d\n", USER_FUNC, led.pin);
            return BSP_ERR;
        }
        
        ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
            ERR("%s gpio_pin_configure_dt configure fail...%d\n", USER_FUNC, led.pin);
            return BSP_ERR;
        }
        LOG("LED port %d initialized\n", led.pin);
    }
    
    
    LOG("%s succeeded\n", USER_FUNC);
    return BSP_OK;
}

int bsp_gpio_toggle(struct gpio_dt_spec led){
    int ret = 0;

    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0) {
        ERR("%s gpio_pin_configure_dt configure fail...%d\n", USER_FUNC, led.pin);
        return BSP_ERR;
    }
    LOG("%s succeeded\n", USER_FUNC);		
    return BSP_OK;
}

int bsp_gpio_set(struct gpio_dt_spec led, int value){
    int ret = 0;

    ret = gpio_pin_set_dt(&led, value);
    if (ret < 0) {
        ERR("%s gpio_pin_set_dt configure fail...%d\n", USER_FUNC, led.pin);
        return BSP_ERR;
    }
    LOG("%s succeeded\n", USER_FUNC);		
    return BSP_OK;
}
