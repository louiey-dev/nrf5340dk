#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H  

#include <zephyr/types.h>

/** @brief User Service UUID. */
#define BT_UUID_USER_VAL \
	BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief Button Characteristic UUID. */
#define BT_UUID_USER_BUTTON_VAL \
	BT_UUID_128_ENCODE(0x00001524, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief LED Characteristic UUID. */
#define BT_UUID_USER_LED_VAL \
	BT_UUID_128_ENCODE(0x00001525, 0x1212, 0xefde, 0x1523, 0x785feabcd123)


#define BT_UUID_USER           BT_UUID_DECLARE_128(BT_UUID_USER_VAL)
#define BT_UUID_USER_BUTTON    BT_UUID_DECLARE_128(BT_UUID_USER_BUTTON_VAL)
#define BT_UUID_USER_LED       BT_UUID_DECLARE_128(BT_UUID_USER_LED_VAL)


/** @brief Callback type for when an LED state change is received. */
typedef void (*led_cb_t)(const bool led_state);

/** @brief Callback type for when the button state is pulled. */
typedef bool (*button_cb_t)(void);

/** @brief Callback struct used by the User Service. */
struct bt_user_cb {
	/** LED state change callback. */
	led_cb_t    led_cb;
	/** Button read callback. */
	button_cb_t button_cb;
};

/** @brief Initialize the User Service.
 *
 * This function registers a GATT service with two characteristics: Button
 * and LED.
 * Send notifications for the Button Characteristic to let connected peers know
 * when the button state changes.
 * Write to the LED Characteristic to change the state of the LED on the
 * board.
 *
 * @param[in] callbacks Struct containing pointers to callback functions
 *			used by the service. This pointer can be NULL
 *			if no callback functions are defined.
 *
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int bt_user_init(struct bt_user_cb *callbacks);

int user_notify(void* buf, uint16_t len);

#endif