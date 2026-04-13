#pragma GCC optimize ("Os")

#include "msx_save.h"

#include <stdlib.h>
#include <string.h>

#include "share/game_save.h"

namespace {

static char* g_romName = nullptr;
static char* g_romPath = nullptr;

static char* dup_str(const char* s)
{
    if (!s || !*s) return nullptr;
    const size_t len = strlen(s) + 1;
    char* out = (char*)malloc(len);
    if (!out) return nullptr;
    memcpy(out, s, len);
    return out;
}

} // namespace

extern "C" void msx_save_set_game_identity(const char* romName, const char* romPath)
{
    free(g_romName);
    free(g_romPath);
    g_romName = dup_str(romName);
    g_romPath = dup_str(romPath);
}

extern "C" void msx_save_clear_game_identity(void)
{
    free(g_romName);
    free(g_romPath);
    g_romName = nullptr;
    g_romPath = nullptr;
}

extern "C" int msx_save_build_path(char* dst, unsigned int dstLen, const char* ext)
{
    if (!dst || dstLen == 0) return 0;

    const char* src = (g_romPath && *g_romPath) ? g_romPath : g_romName;
    if (!share::gameSaveBuildPath(dst, dstLen, "/sd/msx_saves", src, "msx_autosave.rom")) {
        return 0;
    }

    if (ext && *ext) {
        char* dot = strrchr(dst, '.');
        if (dot) {
            *dot = '\0';
        }
        const size_t cur = strlen(dst);
        const size_t extLen = strlen(ext);
        if (cur + extLen + 1 >= dstLen) {
            dst[dstLen - 1] = '\0';
            return 0;
        }
        strcat(dst, ext);
    }

    return 1;
}
