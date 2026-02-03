#pragma once
#include <stdbool.h>
#include <stddef.h>

int str_snprintf(char* dst, size_t dst_size, const char* fmt, ...);

int clamp_i(int v, int lo, int hi);
float clamp_f(float v, float lo, float hi);

bool str_ieq(const char* a, const char* b);