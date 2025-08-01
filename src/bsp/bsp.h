/*



*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/kernel.h>      // for bool
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define LOGD					printf

#define MSG(fmt, args...)     	LOGD("MSG "fmt"", ## args)
#define LOG(fmt, args...)     	LOGD("LOG "fmt"", ## args)
#define ERR(fmt, args...)     	LOGD("ERR "fmt"", ## args)

#define USER_FUNC   __func__

#define FALSE	0
#define	TRUE	1

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct BSP_S
{
   bool isInit;

   /************************************/


}BSP_ST;

enum BSP_ERROR_EN{
    BSP_OK = 0,
    BSP_ERR = -1,
    BSP_NG = -2
};


/* GPIOs */
/* The device tree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BSP_LED0    0
#define BSP_LED1    1
#define BSP_LED2    2
#define BSP_LED3    3

#define NUM_LED		4

static const struct gpio_dt_spec LED0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec LED3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);


#define BTN0_NODE DT_ALIAS(sw0)
#define BTN1_NODE DT_ALIAS(sw1)
#define BTN2_NODE DT_ALIAS(sw2)
#define BTN3_NODE DT_ALIAS(sw3)

#define BSP_BTN1            0
#define BSP_BTN2            1
#define BSP_BTN3            2
#define BSP_BTN4            3
#define BSP_BTN1_MASK       BIT(BSP_BTN1)
#define BSP_BTN2_MASK       BIT(BSP_BTN2)
#define BSP_BTN3_MASK       BIT(BSP_BTN3)
#define BSP_BTN4_MASK       BIT(BSP_BTN4)

#define NUM_BUTTON       4

static const struct gpio_dt_spec BTN0 = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);
static const struct gpio_dt_spec BTN1 = GPIO_DT_SPEC_GET(BTN1_NODE, gpios);
static const struct gpio_dt_spec BTN2 = GPIO_DT_SPEC_GET(BTN2_NODE, gpios);
static const struct gpio_dt_spec BTN3 = GPIO_DT_SPEC_GET(BTN3_NODE, gpios);

typedef void (*bsp_notify_cb_handler_t)(void* buf, uint16_t len);

/* Don't use*/
int bsp_gpio_init(void);
int bsp_gpio_toggle(struct gpio_dt_spec led);
int bsp_gpio_set(struct gpio_dt_spec led, int value);

/* LEDs and Buttons*/
int bsp_button_led_init(bsp_notify_cb_handler_t notify_handler);
int bsp_led_set(int led_offset, bool on);
int bsp_led_toggle(int led_offset);
