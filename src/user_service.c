
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>

#include "user_service.h"
#include "bsp.h"

LOG_MODULE_REGISTER(user_svc, LOG_LEVEL_DBG);

static bool                   notify_enabled;
static bool                   button_state;
static struct bt_user_cb      user_cb;

static void user_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_enabled = (value == BT_GATT_CCC_NOTIFY);
	LOG_DBG("[%s] notify_enabled %d", USER_FUNC, value);
}

static ssize_t user_write_led(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("[%s] Attribute write, handle: %u, conn: %p", USER_FUNC, attr->handle,
		(void *)conn);

	if (len != 1U) {
		LOG_DBG("Write led: Incorrect data length, %d", len);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_DBG("Write led: Incorrect data offset, %d", offset);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	if (user_cb.led_cb) {
		uint8_t val = *((uint8_t *)buf);

		if (val == 0x00 || val == 0x01) {
			user_cb.led_cb(val ? true : false);
			LOG_DBG("Write led: %s", val ? "ON" : "OFF");
		} else {
			LOG_DBG("Write led: Incorrect value");
			return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
		}
	}
	else{
		LOG_ERR("[%s] CB not registered", USER_FUNC);
	}

	return len;
}

static ssize_t user_read_button(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr,
			  void *buf,
			  uint16_t len,
			  uint16_t offset)
{
	const char *value = attr->user_data;

	LOG_DBG("[%s] Attribute read, handle: %u, conn: %p", USER_FUNC, attr->handle,
		(void *)conn);

	if (user_cb.button_cb) {
		button_state = user_cb.button_cb();
		return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
					 sizeof(*value));
	}

	return 0;
}

/* LED Button Service Declaration */
BT_GATT_SERVICE_DEFINE(user_svc,
BT_GATT_PRIMARY_SERVICE(BT_UUID_USER),
	BT_GATT_CHARACTERISTIC(BT_UUID_USER_BUTTON,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, user_read_button, NULL,
			       &button_state),
	BT_GATT_CCC(user_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_USER_LED,
			       BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_WRITE,
			       NULL, user_write_led, NULL),
);

int bt_user_init(struct bt_user_cb *callbacks)
{
	if (callbacks) {
		user_cb.led_cb    = callbacks->led_cb;
		user_cb.button_cb = callbacks->button_cb;
	}

	return 0;
}

int bt_user_send_button_state(bool button_state)
{
	if (!notify_enabled) {
		return -EACCES;
	}
	LOG_DBG("[%s] Sending button state: %d", USER_FUNC, button_state);

	return bt_gatt_notify(NULL, &user_svc.attrs[2],
			      &button_state,
			      sizeof(button_state));
}
