#pragma once
#include <stdint.h>

typedef struct Font5x7Glyph {
  uint8_t rows[7]; // bits 0..4 usados
} Font5x7Glyph;

const Font5x7Glyph* font5x7_get(char c);