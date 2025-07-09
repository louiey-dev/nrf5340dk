/*



*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/kernel.h>      // for bool
#include <zephyr/drivers/gpio.h>

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
/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define NUM_LED		4

static const struct gpio_dt_spec LED0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec LED3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);


int bsp_gpio_init(void);
int bsp_gpio_toggle(struct gpio_dt_spec led);
int bsp_gpio_set(struct gpio_dt_spec led, int value);

