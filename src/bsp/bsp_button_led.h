#include <string.h>
#include <stdint.h>
#include <stdio.h>

enum en_btn_state{
    BTN_WAITING = 0,
    BTN_PRESSED = 1,
};

typedef struct KEY_STATUS_S{
    enum en_btn_state state;  // Button state
    uint16_t pressed_btn;
}KEY_STATUS_ST;

