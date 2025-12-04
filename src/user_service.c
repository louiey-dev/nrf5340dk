
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

static ssize_t user_write_fn(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("[%s] Attribute write, handle: %u, conn: %p, len %d", USER_FUNC, attr->handle,
		(void *)conn, len);

	if(len == 2){
		uint8_t *p = (uint8_t*)buf;
		uint8_t led_num = p[0];
		uint8_t led_state = p[1];
		LOG_DBG("Write led: led_num 0x%x, led_state 0x%x", led_num, led_state);

		bsp_led_set(led_num, led_state ? true : false);
	}else{
		LOG_DBG("rx len is invalid, expected 2 but %d", len);
	}
	return len;
}

static ssize_t user_read_fn(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			  					void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;
	uint8_t rx_buf[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'};

	LOG_DBG("[%s] Attribute read, handle: %u, conn: %p, len %d", USER_FUNC, attr->handle,
		(void *)conn, len);

	LOG_HEXDUMP_INF(buf, sizeof(buf), "Read Buf!");
	LOG_HEXDUMP_INF(value, sizeof(*value), "Read Value!");

	return bt_gatt_attr_read(conn, attr, buf, len, offset, rx_buf, sizeof(rx_buf));
}

/* User Service Declaration */
BT_GATT_SERVICE_DEFINE(user_svc,
BT_GATT_PRIMARY_SERVICE(BT_UUID_USER),
	BT_GATT_CHARACTERISTIC(BT_UUID_USER_BUTTON,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, user_read_fn, NULL,
			       &button_state),
	BT_GATT_CCC(user_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_USER_LED,
			       BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_WRITE,
			       NULL, user_write_fn, NULL),
);

/*!
 @brief        register user callbacks
 @note         
 @param[in]    user callback methods
 @param[out]   
 @return       0 with success
*/
int bt_user_init(struct bt_user_cb *callbacks)
{
	if (callbacks) {
		user_cb.led_cb    = callbacks->led_cb;
		user_cb.button_cb = callbacks->button_cb;
	}

	return 0;
}

int user_notify(void* buf, uint16_t len)
{
	if (!notify_enabled) {
		LOG_ERR("Notify not enabled");
		return -EACCES;
	}
	LOG_DBG("[%s] Notify : %d", USER_FUNC, len);
	LOG_HEXDUMP_INF(buf, len, "Notify Buf");

	return bt_gatt_notify(NULL, &user_svc.attrs[2], buf, len);
}

