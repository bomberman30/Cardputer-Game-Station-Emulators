#pragma GCC optimize ("Ofast")

#include "run_gx4000.h"

#include <M5Cardputer.h>
#include <stdio.h>
#include <string.h>

#include "gx4000_display.h"
#include "gx4000_input.h"
#include "gx4000_sound.h"

extern "C" {
#include "arnold/cpcglob.h"
#include "arnold/cpc.h"     // CPC_Initialise, CPC_SetHardware, CPC_SetRamConfig, CPC_Reset
#include "arnold/arnold.h"  // CPCEmulation_Run, CPCEmulation_Finish
#include "arnold/asic.h"    // Cartridge_Insert, Cartridge_Autostart, Cartridge_Remove
#include "arnold/render.h"  // Render_SetDisplayFullScreen, Render_Finish
#include "arnold/garray.h"  // GateArray_RethinkMemory
#include "arnold/host.h"    // Host_SetGraphicsBufferSurface (our addition)
#include "share/emu_log_cpp.h"
void Host_SetGraphicsBufferSurface(unsigned char *pSurface,
                                   int width, int height, int pitch);
extern unsigned char *pReadRamPtr[8];
extern unsigned char *pWriteRamPtr[8];
extern unsigned char *Z80MemoryBase;
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal function declared in our modified render5.c.
// Returned pointer + geometry let us wire pScreenBase into the host layer.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" void GX4000_Render_GetLineBuffer(unsigned char **ppBuf,
                                             int *pPitch,
                                             int *pXOffset,
                                             int *pRenderW);

// ─────────────────────────────────────────────────────────────────────────────
void run_gx4000(const uint8_t *rom, size_t len, const char *rom_name)
{
    EMU_LOG("[GX4000] ===== GX4000 Start =====\n");
    EMU_LOG("[GX4000] ROM: %s  size=%u bytes\n", rom_name, (unsigned)len);

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.fillScreen(TFT_BLACK);

    // ── Platform layer init ──────────────────────────────────────────────────
    gx4000_display_init();
    gx4000_sound_init(22050);
    gx4000_input_init();

    // ── CPC+ / GX4000 hardware init ──────────────────────────────────────────
    CPC_Initialise();
    CPC_SetFrameSkip(3);                    // render every 4th frame
    CPC_SetHardware(CPC_HW_CPCPLUS);       // GX4000 = Amstrad CPC+
    CPC_SetRamConfig(0);                    // 64 KB base only – no expansion
    CPC_SetCRTCType(3);                     // CPC+ forced CRTC type
    CPC_SetMonitorType(CPC_MONITOR_COLOUR); // colour output

    // ── Rendering init ───────────────────────────────────────────────────────
    if (!Render_SetDisplayFullScreen(GX4000_CPC_VISIBLE_W,
                                      GX4000_CPC_VISIBLE_H,
                                      16))
    {
        EMU_LOG("[GX4000] ERROR: Render_SetDisplayFullScreen failed\n");
        goto cleanup;
    }

    // Retrieve the single-line buffer pointer and wire it into gx4000_host.c
    {
        unsigned char *pBuf   = nullptr;
        int            pitch  = 0;
        int            xoff   = 0;
        int            rw     = 0;
        GX4000_Render_GetLineBuffer(&pBuf, &pitch, &xoff, &rw);
        Host_SetGraphicsBufferSurface(pBuf,
                                       /* width  */ GX4000_CPC_VISIBLE_W,
                                       /* height */ 1,
                                       /* pitch  */ pitch);
        (void)xoff; (void)rw;
    }

    // Enable Arnold audio only after video allocation succeeds
    CPC_SetAudioActive(TRUE);

    // ── Load cartridge ───────────────────────────────────────────────────────
    if (Cartridge_Insert((const unsigned char *)rom,
                          (unsigned long)len) != 0)
    {
        EMU_LOG("[GX4000] ERROR: Cartridge_Insert failed (size=%u)\n",
               (unsigned)len);
        goto cleanup;
    }
    EMU_LOG("[GX4000] Cartridge inserted (%u bytes)\n", (unsigned)len);

    // ── Reset & autostart ────────────────────────────────────────────────────
    CPC_Reset();
    Cartridge_Autostart();   // ASIC unlocks and executes cart entry point

    // ensure Z80 read/write maps are valid before first opcode fetch
    GateArray_RethinkMemory();
    if (Z80MemoryBase == nullptr) {
        EMU_LOG("[GX4000] ERROR: Z80MemoryBase is null\n");
        goto cleanup;
    }
    for (int i = 0; i < 8; ++i) {
        if (pReadRamPtr[i] == nullptr) {
            pReadRamPtr[i] = Z80MemoryBase;
        }
        if (pWriteRamPtr[i] == nullptr) {
            pWriteRamPtr[i] = Z80MemoryBase;
        }
    }

    // ── Main emulation loop ──────────────────────────────────────────────────
    CPCEmulation_Run();

cleanup:
    EMU_LOG("[GX4000] Cleaning up\n");

    Cartridge_Remove();
    CPCEmulation_Finish(); 

    gx4000_sound_shutdown();
    gx4000_display_shutdown();
    gx4000_input_init(); 
}
