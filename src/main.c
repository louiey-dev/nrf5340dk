/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "user_service_init.c"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* EXTERNS */

// LOG_MODULE_REGISTER(nrf5340dk, LOG_LEVEL_DBG);

int main(void)
{
	int err;
	uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 'H', 'e', 'l', 'l', 'o'};
	int exercise_num = 3;

	LOG("My nrf5340dk sample\n");
	LOG("Starting Bluetooth Peripheral User svc sample\n");

	LOG_INF("nRF Connect SDK Fundamentals");
	LOG_INF("Exercise %d", exercise_num);
	LOG_DBG("A log message in debug level");
	LOG_WRN("A log message in warning level!");
	LOG_ERR("A log message in error level!");

	// Hexdump some data
	LOG_HEXDUMP_INF(data, sizeof(data), "Sample Data!");

	/* Initialize the BSP GPIO */
	bsp_button_led_init(button_changed);

	if (IS_ENABLED(CONFIG_BT_USER_SECURITY_ENABLED)) {
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

	err = bt_user_init(&user_callback);
	if (err) {
		ERR("Failed to init user func (err:%d)\n", err);
		return 0;
	}

	k_work_init(&adv_work, adv_work_handler);
	advertising_start();
	LOG("User svc initialized\n");

	while (1) {
		LOG_INF("Alive -> %d", k_uptime_get_32());
		
		k_msleep(SLEEP_TIME_MS*10);
	}
	return 0;
}
