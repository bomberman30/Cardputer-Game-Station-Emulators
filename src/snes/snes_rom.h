#pragma once

#include <stddef.h>
#include <stdint.h>

bool snes_read_title(char* out, size_t outSize, const uint8_t* rom, size_t romSize);
bool isAltGame(const uint8_t* rom, size_t romSize);