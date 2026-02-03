#include "utils/str.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

int str_snprintf(char* dst, size_t dst_size, const char* fmt, ...) {
  if (!dst || dst_size == 0u || !fmt) return -1;
  va_list args;
  va_start(args, fmt);
  const int r = vsnprintf(dst, dst_size, fmt, args);
  va_end(args);
  if (r < 0) return -1;
  if ((size_t)r >= dst_size) {
    dst[dst_size - 1u] = '\0';
    return (int)(dst_size - 1u);
  }
  return r;
}

int clamp_i(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

float clamp_f(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static int tolower_uc(int c) {
  if (c >= 0 && c <= 255) return tolower(c);
  return c;
}

bool str_ieq(const char* a, const char* b) {
  if (!a || !b) return false;
  while (*a && *b) {
    const int ca = tolower_uc((unsigned char)*a);
    const int cb = tolower_uc((unsigned char)*b);
    if (ca != cb) return false;
    ++a; ++b;
  }
  return *a == '\0' && *b == '\0';
}