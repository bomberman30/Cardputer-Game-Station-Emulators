#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void msx_save_set_game_identity(const char* romName, const char* romPath);
void msx_save_clear_game_identity(void);
int msx_save_build_path(char* dst, unsigned int dstLen, const char* ext);

#ifdef __cplusplus
}
#endif
