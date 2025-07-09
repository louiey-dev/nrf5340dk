/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "bsp.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* EXTERNS */
extern struct gpio_dt_spec leds[NUM_LED];

int main(void)
{
	int ret;
	bool led_state = false;
	int led_offset = 0;

	LOG("My nrf5340dk sample\n");

	bsp_gpio_init();

	while (1) {
		/* Toggle the LED */
#if 0		
		ret = bsp_gpio_toggle(leds[led_offset]);
		if (ret < 0) {
			ERR("Failed to toggle LED %d\n", leds[led_offset].pin);
			return ret;
		}

		MSG("nrf5340dk LED%d state: %s\n", leds[led_offset++].pin, led_state ? "ON" : "OFF");
#endif

#if 1
		ret = bsp_gpio_set(leds[led_offset], led_state ? 1 : 0);
		if (ret < 0) {
			ERR("Failed to set LED %d\n", leds[led_offset].pin);
			return ret;
		}

		MSG("nrf5340dk LED%d state: %s\n", leds[led_offset++].pin, led_state ? "ON" : "OFF");
#endif
		if(led_offset >= NUM_LED)
		{
			led_offset = 0;
			led_state = !led_state;
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
