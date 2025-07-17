#include <zephyr/kernel.h>
#include <soc.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <nrfx.h>


static bsp_button_handler_t my_button_handler_cb;

int bsp_button_led_init(){

    return 0;
}
