/********************************************************************

   Created     :   2025.07.31.

   Author      :    Louiey

   Description :    Config LEDs and Buttons for the nRF5340 DK board.
                    LEDs and Buttons defined at .dts file so refer dts
                    Regarding Buttons
                        get interrupt and check whether its level is high
                        start timer, BTN_DETECT_TIME
                        check its level whether it keeps high when expired timer
                        detected button pressed

   History     :

*********************************************************************/
#include <zephyr/kernel.h>
#include <soc.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <nrfx.h>

#include "bsp.h"
#include "bsp_button_led.h"

LOG_MODULE_REGISTER(led_button, LOG_LEVEL_DBG);

#define FLAGS_GPIO_EXP_ACTIVE (GPIO_INT_EDGE | GPIO_INT_HIGH_1 | GPIO_INT_LOW_0 | GPIO_INT_ENABLE)
#define BTN_DETECT_TIME 200

static struct gpio_callback gpio_cb;
static struct k_spinlock lock;
static bsp_button_handler_t btn_handler_cd = NULL;

const struct gpio_dt_spec dk_led[] = { LED0, LED1, LED2, LED3 };
static const struct gpio_dt_spec dk_buttons[] = { BTN0, BTN1, BTN2, BTN3 };

// static enum en_btn_state btn_state = BTN_WAITING;

static void button_pressed(const struct device *gpio_dev, struct gpio_callback *cb, uint32_t pins);
static void btn_timer_expired(struct k_timer *timer_id);

K_TIMER_DEFINE(btn_timer, btn_timer_expired, NULL);

KEY_STATUS_ST key_status = {0};

int bsp_button_led_init(bsp_button_handler_t btn_handler){
    int ret = 0;
    uint32_t pin_mask = 0;

    // register button callback here
    btn_handler_cd = btn_handler;

    // LEDs init
    for(int i = 0; i < NUM_LED; i++) {
        if (!gpio_is_ready_dt(&dk_led[i])) {
            LOG_ERR("LED%d device not ready", i);
        } else {
            ret = gpio_pin_configure_dt(&dk_led[i], GPIO_OUTPUT_ACTIVE);
            if (ret < 0) {
                LOG_ERR("Failed to configure LED%d (err %d)", i, ret);
            }
        }
    }
    LOG("LEDs initialized successfully\n");

    // Buttons init
    for(int i = 0; i < NUM_BUTTON; i++) {
        if (!gpio_is_ready_dt(&dk_buttons[i])) {
            LOG_ERR("Button%d device not ready", i);
        } else {
            ret = gpio_pin_configure_dt(&dk_buttons[i], GPIO_INPUT | GPIO_PULL_UP);
            if (ret < 0) {
                LOG_ERR("Failed to configure Button%d (err %d)", i, ret);
            }
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(dk_buttons); i++) {
		/* Module starts in scanning mode and will switch to
		 * callback mode if no button is pressed.
		 */
		ret = gpio_pin_interrupt_configure_dt(&dk_buttons[i], FLAGS_GPIO_EXP_ACTIVE);
		if (ret) {
			LOG_ERR("Cannot disable callbacks(%d) err(%d)", i, ret);
		}
        LOG_INF("Button pin 0x%08x int enabled", dk_buttons[i].pin);
		pin_mask |= BIT(dk_buttons[i].pin);
	}

    gpio_init_callback(&gpio_cb, button_pressed, pin_mask);

	for (size_t i = 0; i < NUM_BUTTON; i++) {
		ret = gpio_add_callback(dk_buttons[i].port, &gpio_cb);
		if (ret) {
			LOG_ERR("Cannot add callback at button%d (err %d)", i, ret);
		}
	}

    key_status.state = BTN_WAITING;
    key_status.pressed_btn = 0;

    LOG("Buttons initialized successfully\n");

    return 0;
}

int bsp_led_set(int led_offset, bool on)
{
    if (led_offset < 0 || led_offset >= NUM_LED) {
        LOG_ERR("Invalid LED offset: %d", led_offset);
        return -EINVAL;
    }

    int ret = gpio_pin_set_dt(&dk_led[led_offset], on ? 1 : 0);
    if (ret < 0) {
        LOG_ERR("Failed to set LED%d state (err %d)", led_offset, ret);
    }
    return ret;
}

int bsp_led_toggle(int led_offset)
{
    if (led_offset < 0 || led_offset >= NUM_LED) {
        LOG_ERR("Invalid LED offset: %d", led_offset);
        return -EINVAL;
    }

    int ret = gpio_pin_toggle_dt(&dk_led[led_offset]);
    if (ret < 0) {
        LOG_ERR("Failed to toggle LED%d (err %d)", led_offset, ret);
    }
    return ret;
}

int get_buttons(uint32_t btn_num){
    int ret = 0;
    
    switch(btn_num)
    {
        case BSP_BTN1_MASK: ret = BSP_BTN1 + 1; break;
        case BSP_BTN2_MASK: ret = BSP_BTN2 + 1; break;
        case BSP_BTN3_MASK: ret = BSP_BTN3 + 1; break;
        case BSP_BTN4_MASK: ret = BSP_BTN4 + 1; break;
        default:    ret = -1; break;
    }
    return ret;
}

static void button_pressed(const struct device *gpio_dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    uint32_t ret = 0;
    int val = 0;
    int button = 0;

    k_spinlock_key_t key = k_spin_lock(&lock);

    // read buttons
    for (size_t i = 0; i < ARRAY_SIZE(dk_buttons); i++) {
	    val = gpio_pin_get_dt(&dk_buttons[i]);
		if (val < 0) {
			LOG_ERR("Cannot read gpio %d pin", i);
		}
		if (val) {
			ret |= 1U << i;
            // LOG_INF("val 0x%x, ret 0x%x", val, ret);
		}
	}
    button = get_buttons(ret);
    if(button != -1){        
        if(key_status.state == BTN_WAITING && key_status.pressed_btn == 0){
            // first time pressed
            key_status.pressed_btn = button;
            key_status.state = BTN_PRESSED;

            k_timer_start(&btn_timer, K_MSEC(BTN_DETECT_TIME), K_NO_WAIT);
            // LOG("btn + %d pressed\n", key_status.pressed_btn);
        }
    }
    else{
        // LOG_INF("Button released, %d, %d, %d\n", val, key_status.pressed_btn, key_status.state);
        key_status.pressed_btn = 0;
        key_status.state = BTN_WAITING;

        /* check timer status */
        if (k_timer_status_get(&btn_timer) > 0) {
            /* timer has expired */
        } else if (k_timer_remaining_get(&btn_timer) == 0) {
            /* timer was stopped (by someone else) before expiring */
        } else {
            /* timer is still running */
            k_timer_stop(&btn_timer);
        }
    }
	// LOG("Button pressed on GPIO device %s, pins: 0x%08x\r\n", gpio_dev->name, pins);
    k_spin_unlock(&lock, key);
}

static void btn_timer_expired(struct k_timer *timer_id) {
    // Code to be executed when the timer expires
    int val = 0, offset = 0;

    k_spinlock_key_t key = k_spin_lock(&lock);

    if(key_status.pressed_btn > 0)
    {
        offset = key_status.pressed_btn;
        val = gpio_pin_get_dt(&dk_buttons[offset - 1]);
        if(val > 0){
            // ! Button pressed here
            // LOG_INF("Button %d pressed", offset);

            if(btn_handler_cd){
                btn_handler_cd(key_status.pressed_btn, 1);
            }
            else{
            }
        }
        else{
            // LOG_ERR("Button expired with error, %d, %d", key_status.pressed_btn, key_status.state);
        }
        k_spin_unlock(&lock, key);

    // printk("Timer expired!\n");
    }
}
