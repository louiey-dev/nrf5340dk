/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "bsp/bsp.h"
#include "lbs_init.c"
#include <zephyr/logging/log.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* EXTERNS */
extern struct gpio_dt_spec leds[NUM_LED];

LOG_MODULE_REGISTER(nrf5340dk, LOG_LEVEL_DBG);

int main(void)
{
	int ret;
	bool led_state = false;
	int led_offset = 0;
	int err;
	uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 'H', 'e', 'l', 'l', 'o'};
	int exercise_num = 3;

	LOG("My nrf5340dk sample\n");
	LOG("Starting Bluetooth Peripheral LBS sample\n");

	LOG_INF("nRF Connect SDK Fundamentals");
	LOG_INF("Exercise %d", exercise_num);
	LOG_DBG("A log message in debug level");
	LOG_WRN("A log message in warning level!");
	LOG_ERR("A log message in error level!");

	// Hexdump some data
	LOG_HEXDUMP_INF(data, sizeof(data), "Sample Data!");

	/* Initialize the BSP GPIO */
	bsp_gpio_init();

	/* LBS features */
	err = dk_leds_init();
	if (err) {
		printk("LEDs init failed (err %d)\n", err);
		return 0;
	}

	err = init_button();
	if (err) {
		printk("Button init failed (err %d)\n", err);
		return 0;
	}

	if (IS_ENABLED(CONFIG_BT_LBS_SECURITY_ENABLED)) {
		err = bt_conn_auth_cb_register(&conn_auth_callbacks);
		if (err) {
			ERR("Failed to register authorization callbacks.\n");
			return 0;
		}

		err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
		if (err) {
			ERR("Failed to register authorization info callbacks.\n");
			return 0;
		}
	}

	err = bt_enable(NULL);
	if (err) {
		ERR("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	LOG("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_lbs_init(&lbs_callbacs);
	if (err) {
		ERR("Failed to init LBS (err:%d)\n", err);
		return 0;
	}

	k_work_init(&adv_work, adv_work_handler);
	advertising_start();
	LOG("LBS initialized\n");

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

#if 0
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
