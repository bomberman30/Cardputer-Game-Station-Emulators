#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void gx4000_input_init(void);
bool gx4000_input_poll(void);

extern volatile uint8_t gx4000_joy_state;

#ifdef __cplusplus
}
#endif
