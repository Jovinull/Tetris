#pragma once
#include <stdbool.h>

typedef enum LogLevel {
  LOG_DEBUG = 0,
  LOG_INFO  = 1,
  LOG_WARN  = 2,
  LOG_ERROR = 3
} LogLevel;

void log_set_level(LogLevel level);
LogLevel log_get_level(void);

void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_error(const char* fmt, ...);

bool log_is_debug_enabled(void);