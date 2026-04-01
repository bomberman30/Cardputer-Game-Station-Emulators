#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void ngc_save_init(const char* romPathOrName);
void ngc_save_load(void);
void ngc_save_tick(void);
void ngc_save_request_flush(void);
void ngc_save_force_flush(void);
void ngc_save_shutdown(void);

#ifdef __cplusplus
}
#endif