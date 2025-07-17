#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* Button and LED APIs */
/**
 * @typedef bsp_button_handler_t
 * @brief Callback that is executed when a button state change is detected.
 *
 * @param button_state Bitmask of button states.
 * @param has_changed Bitmask that shows which buttons have changed.
 */
typedef void (*bsp_button_handler_t)(uint32_t button_state, uint32_t has_changed);

