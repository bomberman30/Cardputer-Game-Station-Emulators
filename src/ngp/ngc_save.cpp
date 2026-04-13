#include "ngc_save.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "share/game_save.h"

extern "C" {
#include "ngp/race/flash.h"
#include "share/emu_log_cpp.h"
}

/* ============================ Config ============================ */

#define NGC_SAVE_DIR   "/sd/ngp_saves"
#define SAVE_CHECK_MS  2000
#define SAVE_GAP_MS    15000
#define NGC_SAVE_MAGIC   0x5343474EUL /* 'NGCS' */
#define NGC_SAVE_VERSION 1

/* ============================= Etat ============================= */

static char*        g_save_path   = nullptr;
// static TaskHandle_t g_task        = nullptr;
static TickType_t   g_next_check  = 0;
static TickType_t   g_next_allow  = 0;
static TickType_t   g_first_dirty = 0;
static TickType_t   g_last_save   = 0;

/* ============================ Format ============================ */

struct NgcSaveHeader {
  uint32_t magic;
  uint16_t version;
  uint16_t count;
};

struct NgcSaveBlockHeader {
  uint16_t index;     // 0..3 
  uint16_t reserved;
  uint32_t size;
};

/* ========================== Utils path ========================== */

static void make_save_path(const char* romPathOrName) {
  const char* base = share::gameSaveBasename(romPathOrName);
  char name[160] = {0};

  if (base && *base) {
    strncpy(name, base, sizeof(name) - 1);

    char* dot = strrchr(name, '.');
    if (dot) {
      *dot = '\0';
    }

    strncat(name, ".ngs", sizeof(name) - strlen(name) - 1);
  } else {
    strcpy(name, "ngp_autosave.ngs");
  }

  int n = snprintf(g_save_path, PATH_MAX, NGC_SAVE_DIR "/%s", name);
  if (n < 0 || (size_t)n >= PATH_MAX) {
    g_save_path[PATH_MAX - 1] = '\0';
  }
}

static bool save_buf_ready() {
  return (ngpSaveBuf && ngpSaveBufActive);
}

/* ====================== Utils buffer blocks ===================== */

static bool get_window_block_info(int index, size_t* offset, size_t* size) {
  if (!offset || !size) return false;

  switch (index) {
    case 0: *offset = 0x0000; *size = 0x8000; return true; // 32 KB
    case 1: *offset = 0x8000; *size = 0x2000; return true; // 8 KB
    case 2: *offset = 0xA000; *size = 0x2000; return true; // 8 KB
    case 3: *offset = 0xC000; *size = 0x4000; return true; // 16 KB
    default: return false;
  }
}

static int count_dirty_window_blocks() {
  int count = 0;

  for (int i = 0; i < 4; ++i) {
    if (blocksDirty[ngpSaveBufChip][bootBlockStartNum + i]) {
      ++count;
    }
  }

  return count;
}

static void clear_dirty_window_blocks() {
  for (int i = 0; i < 4; ++i) {
    blocksDirty[ngpSaveBufChip][bootBlockStartNum + i] = 0;
  }
}

/* ============================= Save ============================= */

static bool save_now() {
  if (!g_save_path) return false;
  if (!save_buf_ready()) return false;

  if (!share::gameSaveEnsureParentReady(NGC_SAVE_DIR)) {
    EMU_LOG("[NGC][SAVE] storage path not ready, skip save\n");
    return false;
  }

  int dirtyCount = count_dirty_window_blocks();
  if (dirtyCount <= 0) {
    ngpSaveBufDirty = 0;
    return true;
  }

  NgcSaveHeader hdr;
  hdr.magic   = NGC_SAVE_MAGIC;
  hdr.version = NGC_SAVE_VERSION;
  hdr.count   = (uint16_t)dirtyCount;

  share::setGameIsSaving(true);

  FILE* f = fopen(g_save_path, "wb");
  if (!f) {
    share::setGameIsSaving(false);
    EMU_LOG("[NGC][SAVE] open failed for %s\n", g_save_path);
    return false;
  }

  if (fwrite(&hdr, 1, sizeof(hdr), f) != sizeof(hdr)) {
    fclose(f);
    share::setGameIsSaving(false);
    EMU_LOG("[NGC][SAVE] failed writing header\n");
    return false;
  }

  for (int i = 0; i < 4; ++i) {
    if (!blocksDirty[ngpSaveBufChip][bootBlockStartNum + i]) {
      continue;
    }

    size_t offset = 0;
    size_t size = 0;
    if (!get_window_block_info(i, &offset, &size)) {
      fclose(f);
      share::setGameIsSaving(false);
      EMU_LOG("[NGC][SAVE] invalid block index %d\n", i);
      return false;
    }

    NgcSaveBlockHeader bh;
    bh.index = (uint16_t)i;
    bh.reserved = 0;
    bh.size = (uint32_t)size;

    if (fwrite(&bh, 1, sizeof(bh), f) != sizeof(bh)) {
      fclose(f);
      share::setGameIsSaving(false);
      EMU_LOG("[NGC][SAVE] failed writing block header %d\n", i);
      return false;
    }

    if (fwrite(ngpSaveBuf + offset, 1, size, f) != size) {
      fclose(f);
      share::setGameIsSaving(false);
      EMU_LOG("[NGC][SAVE] failed writing block data %d\n", i);
      return false;
    }
  }

  fclose(f);
  share::setGameIsSaving(false);

  clear_dirty_window_blocks();
  ngpSaveBufDirty = 0;
  g_last_save = xTaskGetTickCount();
  g_first_dirty = 0;

  EMU_LOG("[NGC][SAVE] saved %d dirty block(s) to %s\n",
         dirtyCount, g_save_path);
  return true;
}

/* ============================= Load ============================= */

extern "C" void ngc_save_load(void) {
  if (!g_save_path) return;

  if (!save_buf_ready()) {
    EMU_LOG("[NGC][SAVE] skip load (save buffer inactive)\n");
    return;
  }

  if (!share::gameSaveEnsureParentReady(NGC_SAVE_DIR)) {
    EMU_LOG("[NGC][SAVE] skip load (storage not ready)\n");
    return;
  }

  struct stat st;
  if (stat(g_save_path, &st) != 0) {
    EMU_LOG("[NGC][SAVE] no existing save file for %s\n", g_save_path);
    return;
  }

  FILE* f = fopen(g_save_path, "rb");
  if (!f) {
    EMU_LOG("[NGC][SAVE] load open failed for %s\n", g_save_path);
    return;
  }

  NgcSaveHeader hdr;
  if (fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr)) {
    fclose(f);
    EMU_LOG("[NGC][SAVE] failed reading header\n");
    return;
  }

  if (hdr.magic != NGC_SAVE_MAGIC || hdr.version != NGC_SAVE_VERSION) {
    fclose(f);
    EMU_LOG("[NGC][SAVE] invalid save header in %s\n", g_save_path);
    return;
  }

  for (unsigned i = 0; i < hdr.count; ++i) {
    NgcSaveBlockHeader bh;
    if (fread(&bh, 1, sizeof(bh), f) != sizeof(bh)) {
      fclose(f);
      EMU_LOG("[NGC][SAVE] failed reading block header #%u\n", i);
      return;
    }

    size_t offset = 0;
    size_t size = 0;
    if (!get_window_block_info((int)bh.index, &offset, &size)) {
      fclose(f);
      EMU_LOG("[NGC][SAVE] invalid block index %u\n", (unsigned)bh.index);
      return;
    }

    if (bh.size != size) {
      fclose(f);
      EMU_LOG("[NGC][SAVE] invalid block size for index %u (%u != %u)\n",
             (unsigned)bh.index,
             (unsigned)bh.size,
             (unsigned)size);
      return;
    }

    if (fread(ngpSaveBuf + offset, 1, size, f) != size) {
      fclose(f);
      EMU_LOG("[NGC][SAVE] failed reading block data index %u\n",
             (unsigned)bh.index);
      return;
    }
  }

  fclose(f);

  clear_dirty_window_blocks();
  ngpSaveBufDirty = 0;

  EMU_LOG("[NGC][SAVE] loaded save from %s (%ld bytes file)\n",
         g_save_path, (long)st.st_size);
}

/* ============================== API ============================= */

extern "C" void ngc_save_init(const char* romPathOrName) {
  if (!save_buf_ready()) {
    EMU_LOG("[NGC][SAVE] task not started (save buffer inactive)\n");
    return;
  }

  if (!g_save_path) {
    g_save_path = (char*)malloc(PATH_MAX);
    if (!g_save_path) {
      EMU_LOG("[NGC][SAVE] OOM on path alloc, autosave disabled\n");
      return;
    }
  }

  g_save_path[0] = '\0';
  make_save_path(romPathOrName);

  g_next_check  = 0;
  g_next_allow  = 0;
  g_first_dirty = 0;
  g_last_save   = 0;

  EMU_LOG("[NGC][SAVE] path=%s\n", g_save_path);
}

extern "C" void ngc_save_tick(void) {
  if (!ngpSaveBufDirty) return;
  if (!g_save_path) return;
  if (!save_buf_ready()) return;

  TickType_t now = xTaskGetTickCount();
  if (now < g_next_check) return;

  g_next_check = now + pdMS_TO_TICKS(SAVE_CHECK_MS);

  // Direct save logic (sans thread)
  if (g_first_dirty == 0) g_first_dirty = now;
  if (now >= g_next_allow) {
    if (save_now()) {
      g_next_allow = xTaskGetTickCount() + pdMS_TO_TICKS(SAVE_GAP_MS);
    } else {
      EMU_LOG("[NGC][SAVE] save failed, will retry on next tick\n");
    }
  }
}

extern "C" void ngc_save_request_flush(void) {
  save_now();
}

extern "C" void ngc_save_force_flush(void) {
  save_now();
}

extern "C" void ngc_save_shutdown(void) {
  ngc_save_force_flush();

  if (g_save_path) {
    free(g_save_path);
    g_save_path = nullptr;
  }
}