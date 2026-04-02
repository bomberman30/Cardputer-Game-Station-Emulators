#pragma GCC optimize ("Os")

#include <string.h>
#include <Arduino.h>

#include "partitioner.h"
#include "esp_heap_caps.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp_partition.h"

// Sector size for partition table
#define PARTITION_SIZE 4096
static const uint32_t PARTITION_ADDR = 0x00008000;
static const uint32_t PARTITION_SECTOR = PARTITION_ADDR / 0x1000;
static const size_t LAUNCHER_SPIFFS_SIZE = 1 * 1024 * 1024; // 1 MB
static const int MAX_WRITE_RETRIES = 3;

// Gamestation with 4MB of SPIFFS for the Launcher
const uint8_t gamestation[192] PROGMEM = {
    0xAA, 0x50, 0x01, 0x02, 0x00, 0x90, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x6E, 0x76, 0x73, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0x50, 0x00, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x16, 0x00, 0x61, 0x70, 0x70, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0x50, 0x00, 0x10, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x28, 0x00, 0x61, 0x70, 0x70, 0x31,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0x50, 0x01, 0x82, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x40, 0x00, 0x73, 0x70, 0x69, 0x66,
    0x66, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0x50, 0x01, 0x03, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x01, 0x00, 0x63, 0x6F, 0x72, 0x65,
    0x64, 0x75, 0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xEB, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xEA, 0x8A, 0x51, 0x4E, 0xB6, 0x29, 0x5B, 0x5A, 0xAC, 0xC4, 0x21, 0xE7, 0xC7, 0x83, 0xDB, 0x6A
};

static bool build_partition_buffer(uint8_t *buf8) {
    if (!buf8) {
        return false;
    }

    memset(buf8, 0xFF, PARTITION_SIZE);
    memcpy(buf8, gamestation, sizeof(gamestation));
    return true;
}

static bool verify_partition_buffer(const uint8_t *expectedBuf) {
    if (!expectedBuf) {
        printf("[VERIFY][ERROR] expectedBuf is null\n");
        return false;
    }

    uint8_t *readBuf = (uint8_t *)heap_caps_malloc(
        PARTITION_SIZE,
        MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    );
    if (!readBuf) {
        printf("[VERIFY][ERROR] Failed to allocate read buffer\n");
        return false;
    }

    esp_err_t err = spi_flash_read(PARTITION_ADDR, readBuf, PARTITION_SIZE);
    if (err != ESP_OK) {
        printf("[VERIFY][ERROR] spi_flash_read failed: 0x%x\n", err);
        heap_caps_free(readBuf);
        return false;
    }

    for (size_t i = 0; i < PARTITION_SIZE; ++i) {
        if (readBuf[i] != expectedBuf[i]) {
            printf("[VERIFY][ERROR] Mismatch at byte %u: flash=0x%02X expected=0x%02X\n",
                   (unsigned)i, readBuf[i], expectedBuf[i]);
            heap_caps_free(readBuf);
            return false;
        }
    }

    heap_caps_free(readBuf);
    return true;
}

static bool write_gamestation_partition_once(const uint32_t *buf32) {
    if (!buf32) {
        printf("[ROM][ERROR] write buffer is null\n");
        return false;
    }

    printf("[ROM] Erasing sector %u (addr 0x%08X)...\n",
           (unsigned)PARTITION_SECTOR, (unsigned)PARTITION_ADDR);

    int rc = esp_rom_spiflash_erase_sector(PARTITION_SECTOR);
    if (rc != 0) {
        printf("[ROM][ERROR] esp_rom_spiflash_erase_sector failed, rc=%d\n", rc);
        return false;
    }

    printf("[ROM] Writing %u bytes at 0x%08X...\n",
           (unsigned)PARTITION_SIZE, (unsigned)PARTITION_ADDR);

    rc = esp_rom_spiflash_write(PARTITION_ADDR, buf32, PARTITION_SIZE);
    if (rc != 0) {
        printf("[ROM][ERROR] esp_rom_spiflash_write failed, rc=%d\n", rc);
        return false;
    }

    return true;
}

static bool write_gamestation_partition() {
    printf("[ROM] Preparing Game Station partition buffer...\n");

    uint32_t *buf32 = (uint32_t *)heap_caps_malloc(
        PARTITION_SIZE,
        MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT
    );
    if (!buf32) {
        printf("[ROM][ERROR] Failed to allocate partition buffer\n");
        return false;
    }

    uint8_t *buf8 = reinterpret_cast<uint8_t *>(buf32);
    if (!build_partition_buffer(buf8)) {
        printf("[ROM][ERROR] Failed to build partition buffer\n");
        heap_caps_free(buf32);
        return false;
    }

    for (int attempt = 1; attempt <= MAX_WRITE_RETRIES; ++attempt) {
        printf("[ROM] Write attempt %d/%d...\n", attempt, MAX_WRITE_RETRIES);

        if (!write_gamestation_partition_once(buf32)) {
            printf("[ROM][WARN] Write failed on attempt %d\n", attempt);
            continue;
        }

        printf("[ROM] Verifying full sector after attempt %d...\n", attempt);

        if (verify_partition_buffer(buf8)) {
            printf("[ROM] Full sector verification OK.\n");
            heap_caps_free(buf32);
            return true;
        }

        printf("[ROM][WARN] Verification failed after attempt %d, retrying...\n", attempt);
    }

    heap_caps_free(buf32);
    printf("[ROM][ERROR] All write attempts failed.\n");
    return false;
}

// Detect if we are running in Launcher (1 MB SPIFFS)
bool isLauncherLayout() {
    const esp_partition_t *spiffs =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                 ESP_PARTITION_SUBTYPE_DATA_SPIFFS,
                                 "spiffs");

    if (!spiffs) {
        printf("[GUARD] No SPIFFS partition with label 'spiffs' found, aborting.\n");
        return false;
    }

    printf("[GUARD] Current SPIFFS size: 0x%X (%u KB)\n",
           (unsigned)spiffs->size, (unsigned)(spiffs->size / 1024));

    if (spiffs->size != LAUNCHER_SPIFFS_SIZE) {
        printf("[GUARD] SPIFFS size != 1 MiB (launcher layout), aborting.\n");
        return false;
    }

    printf("[GUARD] Launcher layout detected (SPIFFS = 1 MiB).\n");
    return true;
}

bool flashGameStationPartition() {
    printf("Writing Game Station partition (ROM hack)...\n");

    if (!write_gamestation_partition()) {
        printf("[ERROR] Game Station partition write FAILED\n");
        return false;
    }

    printf("[OK] Game Station partition verified successfully.\n");
    return true;
}