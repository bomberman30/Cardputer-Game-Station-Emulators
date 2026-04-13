#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void gx4000_sound_init(int sample_rate);
void gx4000_sound_shutdown(void);
void gx4000_sound_push(const unsigned char *data, unsigned int byte_len);
unsigned long gx4000_sound_take_drop_count(void);

#ifdef __cplusplus
}
#endif
